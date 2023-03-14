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

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#include <opencv2/opencv.hpp>
#include "opencv2/photo.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui.hpp"
using namespace cv;


// namespace AVT {
// namespace VmbAPI {
using namespace AVT;
using namespace VmbAPI;

class FrameObserver : public IFrameObserver {
    public:
    FrameObserver(CameraPtr pCamera) : IFrameObserver(pCamera){};

    void FrameReceived(const FramePtr pFrame)
    {
        m_pCamera->QueueFrame(pFrame);
    }
};


class Camera2 : public CaptureDevice {

    void static PrintCameraInfo( const CameraPtr &camera )
    {
        std::string strID;
        std::string strName;
        std::string strModelName;
        std::string strSerialNumber;
        std::string strInterfaceID;

        std::ostringstream ErrorStream;

        VmbErrorType err = camera->GetID( strID );
        if( VmbErrorSuccess != err )
        {
            ErrorStream << "[Could not get camera ID. Error code: " << err << "("<<AVT::VmbAPI::Examples::ErrorCodeToMessage(err)<<")"<< "]";
            strID =  ErrorStream.str();
        }
                    
        err = camera->GetName( strName );
        if( VmbErrorSuccess != err )
        {
            ErrorStream << "[Could not get camera name. Error code: " << err << "("<<AVT::VmbAPI::Examples::ErrorCodeToMessage(err)<<")"<< "]";
            strName = ErrorStream.str() ;
        }

        err = camera->GetModel( strModelName );
        if( VmbErrorSuccess != err )
        {
            ErrorStream << "[Could not get camera mode name. Error code: " << err << "("<<AVT::VmbAPI::Examples::ErrorCodeToMessage(err)<<")"<< "]";
            strModelName = ErrorStream.str();
        }

        err = camera->GetSerialNumber( strSerialNumber );
        if( VmbErrorSuccess != err )
        {
            ErrorStream << "[Could not get camera serial number. Error code: " << err << "("<<AVT::VmbAPI::Examples::ErrorCodeToMessage(err)<<")"<< "]";
            strSerialNumber = ErrorStream.str();
        }

        err = camera->GetInterfaceID( strInterfaceID );
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
    Camera2(){
        // Get a reference to the VimbaSystem singleton
        std::cout<<"Vimba C++ API Version "<<m_sys<<"\n";           // Print out version of Vimba
        m_sys_err = m_sys.Startup();               // Initialize the Vimba API


        std::stringstream strError;
        if( VmbErrorSuccess == m_sys_err )
        {   
            syson = true;
            m_cam_err = m_sys.GetCameras( m_cameras );            // Fetch all cameras known to Vimba
            if( VmbErrorSuccess == m_cam_err )
            {
                std::cout << "Cameras found: " << m_cameras.size() <<"\n\n";
                camera_number = m_cameras.size();
                // Query all static details of all known cameras and print them out.
                // We don't have to open the cameras for that.
                std::for_each( m_cameras.begin(), m_cameras.end(), PrintCameraInfo );

                // for(int i =0 ; i< 3 ; i++){
                //     checkPixelFormat(m_cameras[i]);
                // }

                // std::cout<<" Format:";
                m_frames = new FramePtrVector(1);
                
                

                frameallocated = true;
                created = true;
            }
            else
            {
                std::cout << "Could not list cameras. Error code: " << m_cam_err << "("<<AVT::VmbAPI::Examples::ErrorCodeToMessage(m_cam_err)<<")"<< "\n";
            }

            // m_sys.Shutdown();                             // Close Vimba
        }
        else
        {
            std::cout << "Could not start system. Error code: " << m_sys_err <<"("<<AVT::VmbAPI::Examples::ErrorCodeToMessage(m_sys_err)<<")"<< "\n";
        }

        
    }

    void checkFeature(CameraPtr cptr){
        FeaturePtr feature;
        double expTime;
        cptr->GetFeatureByName("ExposureTime", feature);
        feature->GetValue(expTime);
        std::cout<<" exp time: "<<expTime<<"us"<<std::endl;
    }

    void checkPixelFormat(CameraPtr cptr){
        FeaturePtr feature;
        std::string strValue;

        cptr->GetFeatureByName("PixelFormat", feature);
        feature->GetValue(strValue);
        std::cout<<strValue<<std::endl;
    }

    void setPixelFormat(CameraPtr cptr){
        FeaturePtr feature;
        cptr->GetFeatureByName("PixelFormat", feature);
        std::string strValue;
        feature->GetValue(strValue);
        std::cout<<"set pixel format to: "<<strValue<<"us"<<std::endl;
        if(std::strcmp(strValue.c_str(), "BayerGR10p") == 0) return;
        else{
        VmbErrorType err = feature->SetValue("BayerGR10p");
        if (VmbErrorSuccess != err){
            std::cout<<"failed to set pixel format"<<std::endl;
            if(VmbErrorApiNotStarted == err)
                std::cout<<"VmbErrorApiNotStarted"<<std::endl;
            if(VmbErrorBadHandle == err)
                std::cout<<"VmbErrorBadHandle"<<std::endl;
            if(VmbErrorInvalidAccess == err)
                std::cout<<"VmbErrorInvalidAccess"<<std::endl;
            if(VmbErrorWrongType == err)
                std::cout<<"VmbErrorWrongType"<<std::endl;
            if(VmbErrorBadParameter == err)
                std::cout<<"VmbErrorBadParameter"<<std::endl;
            std::cout<<"err type:"<<std::hex<<err<<std::dec<<std::endl;
            // std::cout<<"error code to message"<<ErrorCodeToMessage(err)<<std::endl;

        }
        std::string strValue;
        feature->GetValue(strValue);
        std::cout<<"set pixel format to: "<<strValue<<"us"<<std::endl;
        }
    }

    void setExpTime(CameraPtr cptr, double expTime){
        FeaturePtr feature;
        cptr->GetFeatureByName("ExposureTime", feature);
        feature->SetValue(expTime);
        double setExpTime;
        feature->GetValue(setExpTime);
        std::cout<<"set exp time to: "<<setExpTime<<"us"<<std::endl;
    }


    bool shotblack(std::string prefix, int exp, int number, int camera_id=0){
        std::vector<std::string> names;
            std::vector<float> shutter_times;
            for (int id = 0 ; id < number ; id++){
                shot(exp, camera_id);
                std::string name = prefix + "_black_" + std::to_string(id)+".bmp";
                std::this_thread::sleep_for (std::chrono::seconds(1));
                printf("shot token\n");
                names.push_back(name);
                // shutter_times.push_back((float) exp / 1000000.0);
                if(!flush(name.c_str())){
                    printf("\n failed to write image \n");
                } else printf("written_\n");
                
            }

            Mat img = imread(names[0]) * 0.0f;
            // Mat img = imread(names[0]);
            for (int i = 0 ; i < names.size() ; ++i){
                Mat img_temp = imread(names[i]);
                double alpha = i / double(i+1.0);
                double beta = 1.0 / (i+1.0);
                addWeighted(img, alpha, img_temp, beta, 0.0, img);
                // images.push_back(img);
            }

            imwrite(prefix+".bmp", img);

            // Mat scaled_img = img / float(number);?
            
            // imwrite(prefix+"_black.hdr", scaled_img);
            return true;
    }

    void computeHDR(float *image, float *exp){
        for(int i = 0 ; i < m_width * m_height; i++){
            image[i] = image[i] / (exp[i] / 10000.0);
        }
    }



    bool shotHDR(std::string prefix, double start_exp, double scale, int camera_id=0){
            // std::vector<std::string> names;
            // std::vector<float> shutter_times;
            // std::vector<float *> images;

            float *image_final = new float[m_width * m_height];
            float *time_final = new float[m_width * m_height];
            
            for ( int i = 0 ; i < m_width * m_height ; i++){
                image_final[i] = 0.0f;
                time_final[i] = 0.0f;
            }
            // for (int exp = min ; exp < max+1; exp += space){
            //     count += 1;
            // }

        
            // Mat black_img = imread("test_black.bmp");

            // if(camera_id != 0){
            //     black_img = black_img * 0.0f;
            // }
            bool over_exp = true;
            float exp = start_exp;

            while (over_exp){
                shot(exp, camera_id);
                std::string name = prefix + "_exp_" + std::to_string(exp)+".hdr";
                // names.push_back(name);
                // shutter_times.push_back((float) exp / 1000000.0);
                // if(!flush(name.c_str())){
                //     printf("\n failed to write image \n");
                // } else printf("written_\n");
                over_exp = accumulate_valid_buffer(image_final, time_final, exp);
                exp *= scale;
            }

            // printf("load image");
            // std::vector<Mat> images;
            // images.reserve(names.size());
            // for (int i = 0 ; i < names.size() ; ++i){
            //     Mat img = imread(names[i]);
            //     // img = img - black_img;
            //     // img = cv::max(img, 0.0);
            //     images.push_back(img);
            // }
            // printf("cvt to hdr");

            // Ptr<CalibrateDebevec> cal_debevec = createCalibrateDebevec();
            // Mat crf_debevec;
            // cal_debevec->process(images, crf_debevec, shutter_times);
            // imwrite("reponse_curve.hdr", crf_debevec);

            // Mat response = imread("reponse_curve.hdr", IMREAD_UNCHANGED);

            computeHDR(image_final, time_final);
            // Mat hdr;
            // Ptr<MergeDebevec> merge_debevec = createMergeDebevec();
            // merge_debevec->process(images, hdr, shutter_times, response);
            // Save HDR image.
            // imwrite(prefix+"_hdrDebevec.hdr", hdr);
            stbi_write_hdr((prefix+"_mono_hdr.hdr").c_str(), m_width, m_height, 1, image_final);
            printf("write to hdr");
            return true;
    }

    bool shot(double exp=4000, int id=0){
        // std::cout<<"start taking photo of camera"<<id<<std::endl;
        CameraPtr camera_ptr = m_cameras[id];
                // checkPixelFormat(camera_ptr);
        

        // std::cout<<"got camera id"<<std::endl;
        // sys.OpenCameraByID( cameraID.c_str(), VmbAccessModeFull, pCamera ); 

        VmbErrorType err = camera_ptr->Open( VmbAccessModeFull );
        // if can not open
        if ( VmbErrorSuccess != err )
        {
            errorLog(err, "Could not get camera id [error code:" );

            err = camera_ptr->Close();
            errorLog(err, "Could not close camera [error code: " );
            
            return false;
        }

        // get camera id
        // std::string cameraId;
        // err = camera_ptr->GetID( cameraId );
        // std::cout<<"open camera id: "<<cameraId.c_str()<<std::endl;

        if( VmbErrorSuccess != err )
        {
            errorLog(err, "Could not get camera id [error code:" );

            err = camera_ptr->Close();
            errorLog(err, "Could not close camera [error code: " );

            return false;
        }

        // err = camera_ptr->Close();
        setPixelFormat(camera_ptr);

        // camera_ptr->Open( VmbAccessModeFull );
        // checkPixelFormat(camera_ptr);

        setExpTime(camera_ptr, exp);

        VmbInt32_t timeoutvalue = 200;
        err = camera_ptr->AcquireSingleImage((*m_frames)[0],timeoutvalue);
        // printf("Frames:");
        // VmbPixelFormatType ePixelFormat = VmbPixelFormatMono8;
        // VmbErrorType res = (*m_frames)[0]->GetPixelFormat( ePixelFormat );
        // if( VmbErrorSuccess == res )
        // {
        //     std::cout<<"0x"<<std::hex<<ePixelFormat<<std::dec;
        //     std::cout<<std::endl;
        // }
        // else
        // {
        //     std::cout<<"?";
        // }

                // exit(0);

        // printf("%d", err);
        int count = 0;
        while( VmbErrorSuccess != err && count < 10 ){
            err = camera_ptr->AcquireSingleImage((*m_frames)[0],timeoutvalue);
            printf("%d", err);
        }
        if ( VmbErrorSuccess != err ){
            errorLog(err, "timeout! can not capture image " );
        }

        camera_ptr->Close();

        // readbuffer();

        return true;
    }

    void getImageSize(){
        FramePtr fptr = (*m_frames)[0];
        fptr->GetWidth(m_width);
        fptr->GetHeight(m_height);
        fptr->GetImageSize(m_image_buffer_size);
        // std::cout<<"image size in bytes: "<<m_image_buffer_size<<" image width x height "<<m_width*m_height<<" bit per pixel"<<m_image_buffer_size * 8 / (m_width * m_height)<<std::endl;
    }

    void * getImage(int id){
        FramePtr fptr = (*m_frames)[0];
        VmbUchar_t* buffer_ptr;
        fptr->GetImage(buffer_ptr);
        return buffer_ptr;
    }

    int flush(std::string filename, int id=0){
        // FramePtr fptr = (*m_frames)[0];

        // VmbFrameStatusType eReceiveStatus;
        // VmbErrorType err = fptr->GetReceiveStatus( eReceiveStatus );

        // printf(" vmberrorSucess %d, vmbgramestatuscomplete %d", VmbErrorSuccess == err, VmbFrameStatusComplete == eReceiveStatus);
        
        // VmbUchar_t* pBuffer;
        // fptr->GetImage(pBuffer);
        
        // VmbUint32_t width, height;
        // fptr->GetWidth(width);
        // fptr->GetHeight(height);

        return readbuffer(filename);
    }

    bool readbuffer(std::string filename){
        FramePtr fptr = (*m_frames)[0];

        VmbFrameStatusType eReceiveStatus;
        VmbErrorType err = fptr->GetReceiveStatus( eReceiveStatus );
        
        VmbUchar_t* pBuffer;
        fptr->GetImage(pBuffer);

        unsigned int buffersizeInByte;
        fptr->GetImageSize(buffersizeInByte);

        float * pImage;
                
        VmbUint32_t width, height;
        fptr->GetWidth(width);
        fptr->GetHeight(height);

        pImage = new float[width * height];

        unsigned int packet_number = buffersizeInByte / 5;
        int32_t t = 1;
        // std::int64_t test=0;
        // std::memcpy(&test, &t, 4);
        // std::cout<<"\n test memcopy "<<test<<"vs"<<int(t)<<std::endl;
        std::int64_t mask_3 = 1023;
        std::int64_t mask_0 = (mask_3<<30);
        std::int64_t mask_1 = (mask_3<<20);
        std::int64_t mask_2 = (mask_3<<10);
        
        // std::cout<<"mask3 = "<<mask_3<<std::endl;
        // std::cout<<"packet_number "<<packet_number<<std::endl;
        // std::cout<<"mask"<<std::bitset<64>(mask_0)<<" "<<std::bitset<64>(mask_1)<<" "<<std::bitset<64>(mask_2)<<" "<<std::bitset<64>(mask_3)<<" "<<std::dec<<std::endl;

        // unsigned int p_w = width / 2;
        // unsigned int p_h = height / 2;


        for ( int i = 0 ; i < packet_number ; i++){
            // read 5 bytes from bufferInByte
            // unsigned int row = i / p_w;
            // unsigned int col = i % p_w;

            // unsigned int start_idx_row = row * 2;
            // unsigned int start_idx_col = col * 2;
             
            std::int64_t packet;
            std::memcpy(&packet, pBuffer+(i*5), 5); // OK
            
            std::int64_t G = ((packet & mask_0) >> 30);
            std::int64_t R = ((packet & mask_1) >> 20);
            std::int64_t B = ((packet & mask_2) >> 10);
            std::int64_t G2 = (packet & mask_3);

            float G_4 = float(G) / 255.0;
            float R_4 = float(R) / 255.0;
            float B_4 = float(B) / 255.0;
            float G2_4 = int32_t(G2) / 255.0;

            pImage[i * 4] = float(G_4);
            pImage[i * 4 + 1] = float(R_4);
            pImage[i * 4 + 2] = float(B_4);
            pImage[i * 4 + 3] = float(G2_4);
        }

        bool write_err = stbi_write_hdr(filename.c_str(), width, height, 1, pImage);
        delete pImage;
        return write_err;
    }

    void getbuffer(float *dtr){
        FramePtr fptr = (*m_frames)[0];

        VmbFrameStatusType eReceiveStatus;
        VmbErrorType err = fptr->GetReceiveStatus( eReceiveStatus );
        
        VmbUchar_t* pBuffer;
        fptr->GetImage(pBuffer);

        unsigned int buffersizeInByte;
        fptr->GetImageSize(buffersizeInByte);

        // float * pImage;
                
        // VmbUint32_t width, height;
        // fptr->GetWidth(width);
        // fptr->GetHeight(height);

        // pImage = new float[width * height];

        unsigned int packet_number = buffersizeInByte / 5;
        // int32_t t = 1;
        // std::int64_t test=0;
        // std::memcpy(&test, &t, 4);
        // std::cout<<"\n test memcopy "<<test<<"vs"<<int(t)<<std::endl;
        std::int64_t mask_3 = 1023;
        std::int64_t mask_0 = (mask_3<<30);
        std::int64_t mask_1 = (mask_3<<20);
        std::int64_t mask_2 = (mask_3<<10);
        
        // std::cout<<"mask3 = "<<mask_3<<std::endl;
        // std::cout<<"packet_number "<<packet_number<<std::endl;
        // std::cout<<"mask"<<std::bitset<64>(mask_0)<<" "<<std::bitset<64>(mask_1)<<" "<<std::bitset<64>(mask_2)<<" "<<std::bitset<64>(mask_3)<<" "<<std::dec<<std::endl;

        // unsigned int p_w = width / 2;
        // unsigned int p_h = height / 2;


        for ( int i = 0 ; i < packet_number ; i++){
            // read 5 bytes from bufferInByte
            // unsigned int row = i / p_w;
            // unsigned int col = i % p_w;

            // unsigned int start_idx_row = row * 2;
            // unsigned int start_idx_col = col * 2;
             
            std::int64_t packet;
            std::memcpy(&packet, pBuffer+(i*5), 5); // OK
            
            std::int64_t G = ((packet & mask_0) >> 30);
            std::int64_t R = ((packet & mask_1) >> 20);
            std::int64_t B = ((packet & mask_2) >> 10);
            std::int64_t G2 = (packet & mask_3);

            float G_4 = float(G) / 255.0;
            float R_4 = float(R) / 255.0;
            float B_4 = float(B) / 255.0;
            float G2_4 = int32_t(G2) / 255.0;

            dtr[i * 4] = float(G_4);
            dtr[i * 4 + 1] = float(R_4);
            dtr[i * 4 + 2] = float(B_4);
            dtr[i * 4 + 3] = float(G2_4);
        }

        // bool write_err = stbi_write_hdr(filename.c_str(), width, height, 1, pImage);
        // delete pImage;
        // return write_err;
    }


    bool accumulate_valid_buffer(float *image_acc, float *exp_total, float exp){
        FramePtr fptr = (*m_frames)[0];

        VmbFrameStatusType eReceiveStatus;
        VmbErrorType err = fptr->GetReceiveStatus( eReceiveStatus );
        
        VmbUchar_t* pBuffer;
        fptr->GetImage(pBuffer);

        unsigned int buffersizeInByte;
        fptr->GetImageSize(buffersizeInByte);


        unsigned int packet_number = buffersizeInByte / 5;
        std::int64_t mask_3 = 1023;
        std::int64_t mask_0 = (mask_3<<30);
        std::int64_t mask_1 = (mask_3<<20);
        std::int64_t mask_2 = (mask_3<<10);

        bool over_exp = false;


        for ( int i = 0 ; i < packet_number ; i++){             
            std::int64_t packet;
            std::memcpy(&packet, pBuffer+(i*5), 5); // OK
            
            std::int64_t G = ((packet & mask_0) >> 30);
            std::int64_t R = ((packet & mask_1) >> 20);
            std::int64_t B = ((packet & mask_2) >> 10);
            std::int64_t G2 = (packet & mask_3);

            float G_4 = float(G) / 255.0;
            float R_4 = float(R) / 255.0;
            float B_4 = float(B) / 255.0;
            float G2_4 = int32_t(G2) / 255.0;

            over_exp |= (G_4 > 1.0 | R_4 > 1.0 | B_4 >1.0 | G2_4 > 1.0);

            image_acc[i * 4] += (G_4 > 1.0f ? : 0.0f, float(G_4));
            image_acc[i * 4 + 1] += (R_4 > 1.0f ? : 0.0f, float(R_4));
            image_acc[i * 4 + 2] += (B_4 > 1.0f ? : 0.0f, float(B_4));
            image_acc[i * 4 + 3] += (G2_4 > 1.0f ? : 0.0f, float(G2_4));

            exp_total[i * 4] += (G_4 > 1.0f ? : 0.0f, float(exp));
            exp_total[i * 4 + 1] += (R_4 > 1.0f ? : 0.0f, float(exp));
            exp_total[i * 4 + 2] += (B_4 > 1.0f ? : 0.0f, float(exp));
            exp_total[i * 4 + 3] += (G2_4 > 1.0f ? : 0.0f, float(exp));

        }

        return over_exp;
    }

    ~Camera2(){
        if (syson){
            VmbErrorType err = m_sys.Shutdown();
            errorLog(err, "Could not shutdown Vimba [error code: " );
            syson = false;
        }

        if (frameallocated){
            free(m_frames);
            frameallocated = false;
        }
    }

    VmbErrorType m_sys_err;
    VmbErrorType m_cam_err;
    VmbInt64_t nPLS;
    FramePtrVector * m_frames;
    bool syson = false;
    bool frameallocated = false;
    bool created = false;
    unsigned int m_width;
    unsigned int m_height;
    int camera_number = 0;
    unsigned int m_image_buffer_size;

    // Mat black;

    VimbaSystem& m_sys = VimbaSystem::GetInstance();  
    CameraPtrVector m_cameras;                           // A vector of std::shared_ptr<AVT::VmbAPI::Camera> objects
};

// }
// }