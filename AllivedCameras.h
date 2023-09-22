#pragma once
#include "Capture.h"
#include <sstream>
#include <iostream>
#include <vector>
#include <algorithm>
#include <bitset>


#include "VimbaCPP/Include/VimbaCPP.h"
#include "Common/StreamSystemInfo.h"
#include "Common/ErrorCodeToMessage.h"

// #include "stb_image_write.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#include <opencv2/opencv.hpp>
#include "opencv2/photo.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui.hpp"
using namespace cv;
using namespace AVT;
using namespace VmbAPI;

class HDRImage{
    public:
    HDRImage(int width, int height){
        size = width * height;
        photon_buffer = new float[size];
        exp_time = new float[size];
    }

    ~HDRImage(){
        if (photon_buffer != nullptr) delete photon_buffer;
        if (exp_time != nullptr) delete exp_time;
    }

    void setZero(){
        for (unsigned int i = 0 ; i < size ; i++){
            photon_buffer[i] = 0.0;
            exp_time[i] = 0.0;
        }
    }


    void computeHDR(float scale=1.0, float time_unit=1.0){
        for(unsigned int i = 0 ; i < size; i++){
            photon_buffer[i] = photon_buffer[i] * scale / (exp_time[i] / time_unit);
        }
    }

    
    float * photon_buffer;
    float * exp_time;
    unsigned int size;
};

class FrameObserver : public IFrameObserver {
    public:
    //
    // We pass the camera that will deliver the frames to the constructor
    //
    // Parameters:
    //  [in]    pCamera             The camera the frame was queued at

    //
    FrameObserver( CameraPtr pCamera ) : IFrameObserver( pCamera ) {};
    ~FrameObserver(){
        ClearFrameQueue();
        std::cout<<" free pointers "<<std::endl;
    }
    //
    // This is our callback routine that will be executed on every received frame.
    // Triggered by the API.
    //
    // Parameters:
    //  [in]    pFrame          The frame returned from the API
    //
    virtual void FrameReceived( const FramePtr pFrame );
    void ClearFrameQueue();
    void getImageData(unsigned int &w, unsigned int &h);
    void getFirstImage(float *dtr, std::string frame_format);
    std::vector<FramePtr> m_Frames;
};

class Image {
    public:
    // void ClearFrameQueue();
    void addOneImage(uint32_t size){
        if( m_Frames != nullptr)
            delete m_Frames;
        m_image_size = size;
        m_Frames = new float[size];
        std::cout<<"secuss!"<<std::endl;
    }

    ~Image(){
        m_image_size = 0;
        if( m_Frames != nullptr)
            delete m_Frames;
    }

    uint32_t m_image_size = 0;
    float *m_Frames;
};

void FrameObserver::FrameReceived( const FramePtr pFrame ){
    std::cout<<"frame received\n";
    if(! SP_ISNULL( pFrame ) )
    {
        VmbFrameStatusType status;
        VmbErrorType Result;
        Result = SP_ACCESS( pFrame)->GetReceiveStatus( status);
        if( VmbErrorSuccess == Result && VmbFrameStatusComplete == status)
        {
            if(m_Frames.size() <= 0)
                m_Frames.push_back(pFrame);
            std::cout<<"frame complete\n";
        }
        else
        {
            std::cout<<"frame incomplete\n";
        }
    }
    else
    {
        std::cout <<" frame pointer NULL\n";
    }

    SP_ACCESS( m_pCamera )->QueueFrame( pFrame );
    // std::cout<<" camera queue frame "<<std::endl;
}

void FrameObserver::ClearFrameQueue(){
    std::vector<FramePtr> empty;
    std::swap( m_Frames, empty );
}

void FrameObserver::getImageData(unsigned int &width,unsigned int &height){
     if( !m_Frames.empty() ){
        FramePtr fptr = m_Frames[0];
        fptr->GetWidth(width);
        fptr->GetHeight(height);
     }else{
        std::cout<<" no frame captured for camera "<<std::endl;
     }
}

void FrameObserver::getFirstImage(float *dtr, std::string frame_format){
    int frame_id = m_Frames.size()-2;
    if(m_Frames.size() == 1){
        frame_id = 0;
    }

    if( !m_Frames.empty() ){
        FramePtr fptr = m_Frames[frame_id];
        

        VmbFrameStatusType eReceiveStatus;
        VmbErrorType err = SP_ACCESS( fptr )->GetReceiveStatus( eReceiveStatus );
        
        VmbUchar_t* pBuffer;
        SP_ACCESS( fptr )->GetImage(pBuffer);

        unsigned int buffersizeInByte;
        SP_ACCESS( fptr )->GetImageSize(buffersizeInByte);

        std::cout<<"captured"<<std::endl;

        if(frame_format == "BayerGR10"){
            unsigned int pixel = buffersizeInByte / 2;
            for ( int i = 0 ; i < pixel ; i++){
                float pixelValue;
                std::memcpy(&pixelValue, pBuffer+(i*2), 2); // OK
                dtr[i] = pixelValue;
            }

        }else if(frame_format == "BayerGR10p"){
            // std::cout<<"start to copy image"<<packet_number * 4<<std::endl;
            unsigned int packet_number = buffersizeInByte / 5;
            std::cout<<"start to copy image"<<packet_number * 4<<" "<<2592*1944<<std::endl;

            std::int64_t mask_3 = 1023;
            std::int64_t mask_0 = (mask_3<<30);
            std::int64_t mask_1 = (mask_3<<20);
            std::int64_t mask_2 = (mask_3<<10);

            float saturated = 1023.0;
            
            for ( int i = 0 ; i < packet_number ; i++){     
                // std::cout<<"pixel: "        <<i<<std::endl;
                std::int64_t packet;
                std::memcpy(&packet, pBuffer+(i*5), 5); // OK
                
                std::int64_t G = ((packet & mask_0) >> 30);
                std::int64_t R = ((packet & mask_1) >> 20);
                std::int64_t B = ((packet & mask_2) >> 10);
                std::int64_t G2 = (packet & mask_3);

                float G_4 = float(G) / saturated;
                float R_4 = float(R) / saturated;
                float B_4 = float(B) / saturated;
                float G2_4 = int32_t(G2) / saturated;

                dtr[i * 4 + 3] = float(G_4);
                dtr[i * 4 + 2] = float(R_4);
                dtr[i * 4 + 1] = float(B_4);
                dtr[i * 4 + 0] = float(G2_4);
            }
        }

    } else {
        std::cout<<" no frame captured for camera "<<std::endl;
    }   
}

// void Image::ClearFrameQueue(){
   
//     if(m_Frames.size() > 0){
//         std::cout<<" shared pointer "<<m_Frames[0].use_count()<<std::endl;
//         // m_Frames.pop();
//     }
//     std::vector<FramePtr> empty;
//     std::swap( m_Frames, empty );
//     for (int i = 0 ; i < empty.size() ; i++){
//         empty[i].reset();
//     }
    
//     std::cout<<"swap"<<std::endl;
// }


class CameraC : public CaptureDevice {

    void static PrintCameraInfo( const CameraPtr &camera )
    {
        std::string strID;
        std::string strName;
        std::string strModelName;
        std::string strSerialNumber;
        std::string strInterfaceID;

        std::ostringstream ErrorStream;

        VmbErrorType err = SP_ACCESS( camera )->GetID( strID );
        if( VmbErrorSuccess != err )
        {
            ErrorStream << "[Could not get camera ID. Error code: " << err << "("<<AVT::VmbAPI::Examples::ErrorCodeToMessage(err)<<")"<< "]";
            strID =  ErrorStream.str();
        }
                    
        err = SP_ACCESS( camera )->GetName( strName );
        if( VmbErrorSuccess != err )
        {
            ErrorStream << "[Could not get camera name. Error code: " << err << "("<<AVT::VmbAPI::Examples::ErrorCodeToMessage(err)<<")"<< "]";
            strName = ErrorStream.str() ;
        }

        err = SP_ACCESS( camera )->GetModel( strModelName );
        if( VmbErrorSuccess != err )
        {
            ErrorStream << "[Could not get camera mode name. Error code: " << err << "("<<AVT::VmbAPI::Examples::ErrorCodeToMessage(err)<<")"<< "]";
            strModelName = ErrorStream.str();
        }

        err = SP_ACCESS( camera )->GetSerialNumber( strSerialNumber );
        if( VmbErrorSuccess != err )
        {
            ErrorStream << "[Could not get camera serial number. Error code: " << err << "("<<AVT::VmbAPI::Examples::ErrorCodeToMessage(err)<<")"<< "]";
            strSerialNumber = ErrorStream.str();
        }

        err = SP_ACCESS( camera )->GetInterfaceID( strInterfaceID );
        if( VmbErrorSuccess != err )
        {
            ErrorStream << "[Could not get interface ID. Error code: " << err << "("<<AVT::VmbAPI::Examples::ErrorCodeToMessage(err)<<")"<< "]";
            strInterfaceID = ErrorStream.str() ;
        }

        std::cout   << "/// Camera Name    : " << strName           << "\n"
                    << "/// Model Name     : " << strModelName      <<  "\n"
                    << "/// Camera ID      : " << strID             <<  "\n"
                    << "/// Serial Number  : " << strSerialNumber   <<  "\n"
                    << "/// @ Interface ID : " << strInterfaceID    << "\n\n";
    };

    void errorLog(VmbErrorType err, std::string str){
        std::stringstream ss;
        if (VmbErrorSuccess != err){
            ss.str("");
            ss << str << err << "("<<AVT::VmbAPI::Examples::ErrorCodeToMessage(err)<<")"<< "\n";
            std::cout << ss.str() << std::endl;
        }
    }

    public:
    CameraC(): m_sys(VimbaSystem::GetInstance()){
        // Get a reference to the VimbaSystem singleton
        std::cout<<"Vimba C++ API Version "<<m_sys<<"\n";           // Print out version of Vimba
        m_sys_err = m_sys.Startup();                                // Initialize the Vimba API

        // num_frames = nf;
        std::stringstream strError;
        if( VmbErrorSuccess == m_sys_err )
        {   
            syson = true;
            CameraPtrVector m_cameras;
            m_cam_err = m_sys.GetCameras( m_cameras );            // Fetch all cameras known to Vimba
            if( VmbErrorSuccess == m_cam_err )
            {
                std::cout << "Cameras found: " << m_cameras.size() <<"\n\n";
                camera_number = m_cameras.size();

                std::for_each( m_cameras.begin(), m_cameras.end(), PrintCameraInfo );

                created = true;

                m_images.resize(m_cameras.size());
                for( int i  =  0 ; i < m_cameras.size() ; i++){
                    // dynamic_pointer_cast<FrameObserver>(m_pFrameObservers[i]) = new FrameObserver( m_cameras[i] );
                    std::string strID;
                    VmbErrorType err = SP_ACCESS( m_cameras[i] )->GetID( strID );
                    if( VmbErrorSuccess != err )
                    {
                        std::cout << "[Could not get camera ID. Error code: " << err << "("<<AVT::VmbAPI::Examples::ErrorCodeToMessage(err)<<")"<< "]";
                        // strID =  ErrorStream.str();
                        created &= false;
                    }else{
                        m_camera_ids.push_back(strID);
                        created &= true;
                    }
                    
                }
                std::cout<<"---------------------"<<m_width * m_height<<std::endl;
                shot(1000);
                std::cout<<"---------------------"<<m_width * m_height<<std::endl;
                m_hdr_buffer = new HDRImage(m_width, m_height);
                
                std::cout<<"camera created"<<std::endl;
            }
            else
            {
                std::cout << "Could not list cameras. Error code: " << m_cam_err << "("<<AVT::VmbAPI::Examples::ErrorCodeToMessage(m_cam_err)<<")"<< "\n";
            }

        }
        else
        {
            std::cout << "Could not start system. Error code: " << m_sys_err <<"("<<AVT::VmbAPI::Examples::ErrorCodeToMessage(m_sys_err)<<")"<< "\n";
        }
        m_sys.Shutdown();
        
    }

    void setPixelFormat(CameraPtr cptr){
        FeaturePtr feature;
        cptr->GetFeatureByName("PixelFormat", feature);
        std::string strValue;
        feature->GetValue(strValue);
        std::cout<<"set pixel format from: "<<strValue<<" to "<<m_frame_format.c_str()<<std::endl;
        if(std::strcmp(strValue.c_str(), m_frame_format.c_str()) == 0) return;
        else{ VmbErrorType err = feature->SetValue(m_frame_format.c_str());
            if (VmbErrorSuccess != err) errorLog(err, "failed to set pixel format");
            std::string strValue;
            feature->GetValue(strValue);
            std::cout<<"set pixel format to: "<<strValue<<"us"<<std::endl;
        }
    }

    void AdjustBandLimit(CameraPtr cptr){
        FeaturePtr bandlimit;
        SP_ACCESS( cptr )->GetFeatureByName("DeviceLinkThroughputLimit", bandlimit);

        long long int max_band, min_band;
        SP_ACCESS( bandlimit )->GetRange( min_band, max_band );
        long long int seto = (2 * min_band);
        SP_ACCESS( bandlimit )->SetValue(seto);
        long long int after_set;
        SP_ACCESS( bandlimit )->GetValue(after_set);
        std::cout<<"band: "<<min_band<<"band: "<<after_set<<std::endl;
    }

    // void DisableAutoExposure(CameraPtr cptr){
    //     FeaturePtr auto_exp;
    //     SP_ACCESS( cptr )->
    // }

    void setExpTime(CameraPtr cptr, double expTime){
        FeaturePtr feature;
        SP_ACCESS( cptr )->GetFeatureByName("ExposureTime", feature);
        double min_exp, max_exp;
        feature->GetRange( min_exp, max_exp );
        std::cout<<"supprt exp: "<<min_exp<<" max: "<<max_exp<<std::endl;

        feature->SetValue(expTime);
        double setExpTime;
        feature->GetValue(setExpTime);
        std::cout<<"set exp time to: "<<setExpTime<<"us"<<std::endl;
        sleep(2);
    }



    bool findSaturated(std::string prefix, double max_exp, double min_exp, double step, int camera_id=0){

        bool over_exp = true;
        float exp = max_exp;

        
        while (exp > min_exp){
            shot(exp, camera_id);
            std::string name = prefix + "_exp_" + std::to_string(exp)+".hdr";
            readbuffer(name, camera_id);
            exp -= step;
        }

        return true;
    }



    bool shotHDR(std::string prefix, double start_exp, double scale, int camera_id=0){
            m_hdr_buffer->setZero();
            bool over_exp = true;
            float exp = start_exp;
            int i = 0;
            while (over_exp){
                bool suc = shot(exp, camera_id);
                assert(suc == true);
                std::string name = prefix + "_exp_" + std::to_string(exp)+".hdr";
                stbi_write_hdr(name.c_str(), m_width, m_height, 1, m_images[camera_id].m_Frames);
                over_exp = accumulate_valid_buffer(m_hdr_buffer->photon_buffer, m_hdr_buffer->exp_time, exp, camera_id);
                exp *= scale;
            }
            m_hdr_buffer->computeHDR(1.0, 1000000.0);
            stbi_write_hdr((prefix+"_mono_hdr.hdr").c_str(), m_width, m_height, 1, m_hdr_buffer->photon_buffer);
            return true;
    }

    bool shot(double exp=400000, int id=0){
        std::cout<<"start taking photo of camera "<<id<<std::endl;
        m_sys_err = m_sys.Startup();                                // Initialize the Vimba API
        if (m_sys_err == VmbErrorSuccess){
            int patient = 0;
            while (true & patient < 100){
                m_cam_err = m_sys.OpenCameraByID( m_camera_ids[id].c_str(), VmbAccessModeFull, camera_ptr );
                if (VmbErrorSuccess == m_cam_err) break;
                else {
                    errorLog(m_cam_err, "Could not get camera id, try again [error code: " );
                    m_cam_err = SP_ACCESS( camera_ptr )->Close();
                    errorLog(m_cam_err, "Could not close camera [error code: " );
                    patient += 1;
                }
            }
        
            if ( VmbErrorSuccess == m_cam_err )
            {
                // PrepareCamera 
                setPixelFormat(camera_ptr);
                AdjustBandLimit(camera_ptr);
                setExpTime(camera_ptr, exp);

                int num_frames = int(1000000 / exp) + 3;
                std::cout<<" [Shot]: start aquisition of"<<num_frames<<" frames"<<std::endl;

                
                SP_SET( tempFrameObserver , new FrameObserver( camera_ptr ) );

                // Start streaming
                m_cam_err = SP_ACCESS( camera_ptr )->StartContinuousImageAcquisition( num_frames, tempFrameObserver, FrameAllocation_AnnounceFrame );
                std::cout<<" [Shot]: started "<<std::endl;

                if( VmbErrorSuccess != m_cam_err){
                    errorLog(m_cam_err, " [Shot]: failed to start continuous image acquisition [error code: ");
                    SP_ACCESS( camera_ptr )->Close();
                    return false;
                }

                // sleep for some amount of time  and then
                
                double seconds = exp * num_frames / 1000000;
                int sleep_sec = int(seconds);
                std::cout<<" [Shot]: sleep "<<sleep_sec<<" sec"<<std::endl;
                std::this_thread::sleep_for (std::chrono::seconds(sleep_sec));

                std::cout<<" [Shot]: finish sleep "<<sleep_sec<<" sec"<<std::endl;
                m_cam_err = SP_ACCESS( camera_ptr )->StopContinuousImageAcquisition();

                std::cout<<" [Shot]: stop aquisition "<<sleep_sec<<" sec"<<std::endl;
                
                if( VmbErrorSuccess == m_cam_err){
                    // check the grame
                    if(! SP_DYN_CAST( tempFrameObserver, FrameObserver )->m_Frames.empty()){
                        std::cout<<" captured "<<SP_DYN_CAST( tempFrameObserver, FrameObserver )->m_Frames.size()<<" images with "<<exp<<" exposure us\n "; 
                        SP_DYN_CAST( tempFrameObserver, FrameObserver )->getImageData(m_width, m_height);
                        std::cout<<" image size "<<m_width<<"x"<<m_height<<std::endl;
                        m_images[id].addOneImage(m_width * m_height);
                        std::cout<<" init one image "<<std::endl;
                        // int fid = m_images[id].m_Frames.size() - 1;
                        SP_DYN_CAST( tempFrameObserver, FrameObserver )->getFirstImage(m_images[id].m_Frames, m_frame_format);
                        std::cout<<" got first image "<<std::endl;
                        SP_ACCESS(camera_ptr)->Close();
                        SP_DYN_CAST( tempFrameObserver, FrameObserver )->ClearFrameQueue();
                        SP_RESET( tempFrameObserver);
                        std::cout<<" camera closed "<<std::endl;
                        SP_RESET( camera_ptr);
                        return true;
                    }else{
                        std::cout<<" time is too short to capture a image with exposure: "<<exp<<" us\n "; 
                        SP_ACCESS(camera_ptr)->Close();
                        return false;
                    }
                }else{
                    errorLog(m_cam_err, " [Shot]: failed to stop continuous image acquisition [error code: ");
                    SP_ACCESS( camera_ptr )->Close();
                    return false;
                }
            }else{
                errorLog(m_cam_err, "Could not get camera id [error code:" );
                m_cam_err = SP_ACCESS( camera_ptr )->Close();
                errorLog(m_cam_err, "Could not close camera [error code: " );
                return false;
            }
            std::cout<<"try to shot down"<<std::endl;
            m_sys.Shutdown();
            
            std::cout<<"system shot down"<<std::endl;
        }else{
            errorLog(m_sys_err, "Could not open system [error code:" );
            return false;
        }
    }

    // bool getImageSize(int camera_id){
    //     if( !m_images[camera_id].m_Frames.empty() ){
    //         FramePtr fptr = m_images[camera_id].m_Frames[0];
    //         fptr->GetWidth(m_width);
    //         fptr->GetHeight(m_height);
    //         fptr->GetImageSize(m_image_buffer_size);
    //         return true;
    //     }else{
    //         std::cout<<" No image captured for camera "<<camera_id<<m_images[camera_id].m_Frames.size()<<std::endl;
    //         return false;
    //     }    
    // }

    bool readbuffer(std::string filename, int camera_id){
        if( m_images[camera_id].m_image_size != 0 ){
            // FramePtr fptr = m_images[camera_id].m_Frames[0];
            
            // VmbUchar_t* pBuffer;
            // fptr->GetImage(pBuffer);

            // unsigned int buffersizeInByte;
            // fptr->GetImageSize(buffersizeInByte);

            // std::cout<<"buffer size in byte"<<buffersizeInByte<<std::endl;

            // float * pImage;
                    
            // VmbUint32_t width, height;
            // fptr->GetWidth(width);
            // fptr->GetHeight(height);

            // pImage = new float[width * height];

            // if(m_frame_format == "BayerGR10"){
            //     unsigned int pixel = buffersizeInByte / 2;

            //     float saturated = 1023.0;
            //     // std::int64_t mask = 1023;
            //     for ( int i = 0 ; i < pixel ; i++){
            //         float pixelValue;
            //         std::memcpy(&pixelValue, pBuffer+(i*2), 2); // OK
            //         float px = float(pixelValue) / saturated;
            //         pImage[i] = px;
            //     }

            // }else if(m_frame_format == "BayerGR10p"){

            //     unsigned int packet_number = buffersizeInByte / 5;

            //     std::int64_t mask_3 = 1023;
            //     std::int64_t mask_0 = (mask_3<<30);
            //     std::int64_t mask_1 = (mask_3<<20);
            //     std::int64_t mask_2 = (mask_3<<10);

            //     float saturated = 1023.0;
                
            //     for ( int i = 0 ; i < packet_number ; i++){             
            //         std::int64_t packet;
            //         std::memcpy(&packet, pBuffer+(i*5), 5); // OK
                    
            //         std::int64_t G = ((packet & mask_0) >> 30);
            //         std::int64_t R = ((packet & mask_1) >> 20);
            //         std::int64_t B = ((packet & mask_2) >> 10);
            //         std::int64_t G2 = (packet & mask_3);

            //         float G_4 = float(G) / saturated;
            //         float R_4 = float(R) / saturated;
            //         float B_4 = float(B) / saturated;
            //         float G2_4 = int32_t(G2) / saturated;

            //         pImage[i * 4 + 3] = float(G_4);
            //         pImage[i * 4 + 2] = float(R_4);
            //         pImage[i * 4 + 1] = float(B_4);
            //         pImage[i * 4 + 0] = float(G2_4);
            //     }
            // }

            bool write_err = stbi_write_hdr(filename.c_str(), m_width, m_height, 1, m_images[camera_id].m_Frames);
            // delete pImage;
            return write_err;
        } else {
            std::cout<<" no frame captured for camera "<<camera_id<<std::endl;
            return false;
        }
        
    }

    void getbuffer(float *dtr, int camera_id){
        if( m_images[camera_id].m_image_size > 0 ){
            // FramePtr fptr = m_images[camera_id].m_Frames[0];
            std::memcpy(dtr, m_images[camera_id].m_Frames, m_width * m_height * sizeof(float));

            // VmbFrameStatusType eReceiveStatus;
            // VmbErrorType err = fptr->GetReceiveStatus( eReceiveStatus );
            
            // VmbUchar_t* pBuffer;
            // fptr->GetImage(pBuffer);

            // unsigned int buffersizeInByte;
            // fptr->GetImageSize(buffersizeInByte);

            // if(m_frame_format == "BayerGR10"){
            //     unsigned int pixel = buffersizeInByte / 2;
            //     // std::int64_t mask = 1023;
            //     for ( int i = 0 ; i < pixel ; i++){
            //         float pixelValue;
            //         std::memcpy(&pixelValue, pBuffer+(i*2), 2); // OK
            //         dtr[i] = pixelValue;
            //     }

            // }else if(m_frame_format == "BayerGR10p"){

            //     unsigned int packet_number = buffersizeInByte / 5;

            //     std::int64_t mask_3 = 1023;
            //     std::int64_t mask_0 = (mask_3<<30);
            //     std::int64_t mask_1 = (mask_3<<20);
            //     std::int64_t mask_2 = (mask_3<<10);

            //     float saturated = 1023.0;
                
            //     for ( int i = 0 ; i < packet_number ; i++){             
            //         std::int64_t packet;
            //         std::memcpy(&packet, pBuffer+(i*5), 5); // OK


                    
            //         std::int64_t G = ((packet & mask_0) >> 30);
            //         std::int64_t R = ((packet & mask_1) >> 20);
            //         std::int64_t B = ((packet & mask_2) >> 10);
            //         std::int64_t G2 = (packet & mask_3);

            //         float G_4 = float(G) / saturated;
            //         float R_4 = float(R) / saturated;
            //         float B_4 = float(B) / saturated;
            //         float G2_4 = int32_t(G2) / saturated;

            //         dtr[i * 4 + 3] = float(G_4);
            //         dtr[i * 4 + 2] = float(R_4);
            //         dtr[i * 4 + 1] = float(B_4);
            //         dtr[i * 4 + 0] = float(G2_4);
            //     }
            // }

        } else {
            std::cout<<" no frame captured for camera "<<camera_id<<std::endl;
        }
    }


    bool accumulate_valid_buffer(float *image_acc, float *exp_total, float exp, int camera_id){
        std::cout<<"accumulate valid buffer"<<std::endl;
        // FramePtr fptr = m_images[camera_id].m_Frames[0];
        // FramePtr fptr = (*m_frames)[0];

        // VmbFrameStatusType eReceiveStatus;
        // VmbErrorType err = fptr->GetReceiveStatus( eReceiveStatus );
        
        // VmbUchar_t* pBuffer;
        // fptr->GetImage(pBuffer);

        // unsigned int buffersizeInByte;
        // fptr->GetImageSize(buffersizeInByte);
        bool over_exp = false;
        // int frame_id = m_images[camera_id].m_Frames.size()-1;
        // if(m_frame_format == "BayerGR10"){
        unsigned int pixel = m_width * m_height;
            // float saturated = 1023.0;
        for ( unsigned int i = 0 ; i < pixel ; i++){
            // std::cout<<"camera_id"<<camera_id<<"pxiel"<<i<<"in"<<pixel<<" size"<<m_images[camera_id].m_image_size<<std::endl;
            // std::cout<<""<<std::endl;
            // std::cout<<(m_images[camera_id].m_Frames == nullptr)<<std::endl;
            float px = m_images[camera_id].m_Frames[i];
            // std::cout<<"pixel value"<<px<<std::endl;
            // std::cout<<"before"<<std::endl;
            over_exp |= (px >= 1.0);
            // if (px >= 1.0f) over_exp = true;
            // std::cout<<"after"<<std::endl;
            // std::cout<<"writing"<<std::endl;
            image_acc[i] += (px>=1.0f? 0.0f: px);
            exp_total[i] += (px>=1.0f? 0.0f: float(exp));
        }
        //     }

        // }else if(m_frame_format == "BayerGR10p"){
        //     // unsigned int packet_number = buffersizeInByte / 5;
        //     // std::int64_t mask_3 = 1023;
        //     // std::int64_t mask_0 = (mask_3<<30);
        //     // std::int64_t mask_1 = (mask_3<<20);
        //     // std::int64_t mask_2 = (mask_3<<10);

        //     float saturated = 1023.0;
        //     for ( int i = 0 ; i < packet_number ; i++){             
        //         std::int64_t packet;
        //         std::memcpy(&packet, pBuffer+(i*5), 5); // OK
                
        //         std::int64_t G = ((packet & mask_0) >> 30);
        //         std::int64_t R = ((packet & mask_1) >> 20);
        //         std::int64_t B = ((packet & mask_2) >> 10);
        //         std::int64_t G2 = (packet & mask_3);

        //         float saturated = 1023.0 - 1;

        //         float G_4 = float(G) / saturated;
        //         float R_4 = float(R) / saturated;
        //         float B_4 = float(B) / saturated;
        //         float G2_4 = int32_t(G2) / saturated;

        //         over_exp |= (G_4 >= 1.0 | R_4 >= 1.0 | B_4 >= 1.0 | G2_4 >= 1.0);

        //         image_acc[i * 4 + 3] += (G_4 >= 1.0f ? 0.0f: float(G_4));
        //         image_acc[i * 4 + 2] += (R_4 >= 1.0f ? 0.0f: float(R_4));
        //         image_acc[i * 4 + 1] += (B_4 >= 1.0f ? 0.0f: float(B_4));
        //         image_acc[i * 4 ] += (G2_4 >= 1.0f ? 0.0f: float(G2_4));

        //         exp_total[i * 4 + 3] += (G_4 >= 1.0f ? 0.0f: float(exp));
        //         exp_total[i * 4 + 2] += (R_4 >= 1.0f ? 0.0f: float(exp));
        //         exp_total[i * 4 + 1] += (B_4 >= 1.0f ? 0.0f: float(exp));
        //         exp_total[i * 4] += (G2_4 >= 1.0f ? 0.0f: float(exp));

        //     }
        // }
        std::cout<<"return from accumuluate"<<std::endl;
        return over_exp;
    }

    ~CameraC(){
        // m_sys.Shutdown();
        // for( int i = 0 ; i < m_images.size() ; i++){
        //     m_images[i].ClearFrameQueue();
        // }
        delete m_hdr_buffer;
    }

    VmbErrorType m_sys_err;
    VmbErrorType m_cam_err;

   
    bool syson = false;
    bool frameallocated = false;
    bool created = false;

    unsigned int m_width;
    unsigned int m_height;
    int camera_number = 0;
    // int num_frames = 3;
    unsigned int m_image_buffer_size;
    std::string m_frame_format = "BayerGR10p";

    CameraPtr camera_ptr;
    IFrameObserverPtr tempFrameObserver;
    // std::vector<IFrameObserverPtr>    m_pFrameObservers;
    std::vector<Image> m_images;           // Every camera has its own frame observer
    VimbaSystem& m_sys;// = VimbaSystem::GetInstance();  
    // CameraPtrVector m_cameras;
    std::vector<std::string> m_camera_ids;
    HDRImage *m_hdr_buffer;
};

