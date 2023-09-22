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

// class FrameObserver : public IFrameObserver {
//     public:
//     FrameObserver(CameraPtr pCamera) : IFrameObserver(pCamera){};

//     void FrameReceived(const FramePtr pFrame)
//     {
//         SP_ACCESS( pCamera )->QueueFrame(pFrame);
//     }
// };


class Camera2 : public CaptureDevice {

    void static PrintFeatureValue( const FeaturePtr &feature )
    {
        VmbFeatureDataType  eType;
        VmbErrorType        err     = SP_ACCESS( feature )->GetDataType( eType );
        if( VmbErrorSuccess != err )
        {
            std::cout << "[Could not get feature Data Type. Error code: " << err << " (" << AVT::VmbAPI::Examples::ErrorCodeToMessage( err ) << ")" << "]\n";
        }
        else
        {
            std::cout << "/// Value          : ";
            switch( eType )
            {
                case VmbFeatureDataBool:
                    {
                        VmbBool_t bValue;
                        err = SP_ACCESS( feature )->GetValue( bValue );
                        // if ( VmbErrorSuccess == PrintGetValueErrorMessage( err ) )
                        {
                            std::cout << bValue << "\n";
                        }
                        std::cout << "/// Type           : Boolean\n";
                    }
                    break;
                case VmbFeatureDataEnum:
                    {
                        std::string strValue;
                        err = SP_ACCESS( feature )->GetValue( strValue );
                        // if ( VmbErrorSuccess == PrintGetValueErrorMessage( err ) )
                        {
                            std::cout << strValue << "\n";
                        }
                        std::cout << "/// Type           : Enumeration\n";
                    }
                    break;
                case VmbFeatureDataFloat:
                    {
                        double fValue;
                        double fMin, fMax;
                        err = SP_ACCESS( feature )->GetValue( fValue );
                        // if( VmbErrorSuccess == PrintGetValueErrorMessage( err ) )
                        {
                            std::cout << fValue << "\n";
                        }

                        std::cout << "/// Minimum        : ";
                        err = SP_ACCESS( feature )->GetRange( fMin, fMax );
                        // if( VmbErrorSuccess == PrintGetValueErrorMessage( err ) )
                        {
                            std::cout << fMin << "\n";
                            std::cout << "/// Maximum        : " << fMax << "\n";
                        }
                        std::cout << "/// Type           : Double precision floating point\n";
                    }
                    break;
                case VmbFeatureDataInt:
                    {
                        VmbInt64_t nValue;
                        VmbInt64_t nMin, nMax;
                        err = SP_ACCESS( feature )->GetValue( nValue );
                        // if( VmbErrorSuccess == PrintGetValueErrorMessage( err ) )
                        {
                            std::cout << nValue << "\n";
                        }

                        std::cout << "/// Minimum        : ";
                        err = SP_ACCESS( feature )->GetRange( nMin, nMax );
                        // if( VmbErrorSuccess == PrintGetValueErrorMessage( err ) )
                        {
                            std::cout << nMin << "\n";
                            std::cout << "/// Maximum        : " << nMax << "\n";
                        }
                        std::cout << "/// Type           : Long long integer\n";
                    }
                    break;
                case VmbFeatureDataString:
                    {
                        std::string strValue;
                        
                        err = SP_ACCESS( feature )->GetValue( strValue );
                        // if( VmbErrorSuccess == PrintGetValueErrorMessage( err ) )
                        {
                            std::cout << strValue << "\n";
                        }
                        std::cout << "/// Type           : String\n";
                    }
                    break;
                case VmbFeatureDataCommand:
                default:
                    {
                        std::cout << "[None]" << "\n";
                        std::cout << "/// Type           : Command feature\n";
                    }
                    break;
            }

            std::cout << "\n";
        }
    }

        //
    // Prints out all details of a feature
    //
    // Parameters:
    //  [in]    feature         The feature to work on
    //
    void static PrintFeatures( const FeaturePtr &feature )
    {
        std::string strName;                                                    // The name of the feature
        std::string strDisplayName;                                             // The display name of the feature
        std::string strToolTip;                                                 // A short description of the feature
        std::string strDescription;                                             // A long description of the feature
        std::string strCategory;                                                // A category to group features
        std::string strSFNCNamespace;                                           // The Standard Feature Naming Convention namespace
        std::string strUnit;                                                    // The measurement unit of the value

        std::ostringstream ErrorStream;

        VmbErrorType err = SP_ACCESS( feature )->GetName( strName );
        if( VmbErrorSuccess != err )
        {
            ErrorStream << "[Could not get feature Name. Error code: " << err << " (" << AVT::VmbAPI::Examples::ErrorCodeToMessage( err ) << ")" << "]";
            strName = ErrorStream.str();
        }

        err = SP_ACCESS( feature )->GetDisplayName( strDisplayName );
        if( VmbErrorSuccess != err )
        {
            ErrorStream << "[Could not get feature Display Name. Error code: " << err << " (" << AVT::VmbAPI::Examples::ErrorCodeToMessage( err ) << ")" << "]";
            strDisplayName = ErrorStream.str();
        }

        err = SP_ACCESS( feature )->GetToolTip( strToolTip );
        if( VmbErrorSuccess != err )
        {
            ErrorStream << "[Could not get feature Tooltip. Error code: " << err << " (" << AVT::VmbAPI::Examples::ErrorCodeToMessage( err ) << ")" << "]";
            strToolTip = ErrorStream.str();
        }

        err = SP_ACCESS( feature )->GetDescription( strDescription );
        if( VmbErrorSuccess != err )
        {
            ErrorStream << "[Could not get feature Description. Error code: " << err << " (" << AVT::VmbAPI::Examples::ErrorCodeToMessage( err ) << ")" << "]";
            strDescription = ErrorStream.str();
        }

        err = SP_ACCESS( feature )->GetCategory( strCategory );
        if( VmbErrorSuccess != err )
        {
            ErrorStream << "[Could not get feature Category. Error code: " << err << " (" << AVT::VmbAPI::Examples::ErrorCodeToMessage( err ) << ")" << "]";
            strCategory = ErrorStream.str();
        }

        err = SP_ACCESS( feature )->GetSFNCNamespace( strSFNCNamespace );
        if( VmbErrorSuccess != err )
        {
            ErrorStream << "[Could not get feature SNFC Namespace. Error code: " << err << " (" << AVT::VmbAPI::Examples::ErrorCodeToMessage( err ) << ")" << "]";
            strSFNCNamespace = ErrorStream.str();
        }

        err = SP_ACCESS( feature )->GetUnit( strUnit );
        if( VmbErrorSuccess != err )
        {
            ErrorStream << "[Could not get feature Unit. Error code: " << err << " (" << AVT::VmbAPI::Examples::ErrorCodeToMessage( err ) << ")" << "]";
            strUnit = ErrorStream.str();
        }

        std::cout << "/// Feature Name   : " << strName             << "\n";
        std::cout << "/// Display Name   : " << strDisplayName      << "\n";
        std::cout << "/// Tooltip        : " << strToolTip          << "\n";
        std::cout << "/// Description    : " << strDescription      << "\n";
        std::cout << "/// SNFC Namespace : " << strSFNCNamespace    << "\n";
        std::cout << "/// Unit           : " << strUnit             << "\n";
        
        PrintFeatureValue( feature );
    }

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
    Camera2(){
        // Get a reference to the VimbaSystem singleton
        std::cout<<"Vimba C++ API Version "<<m_sys<<"\n";           // Print out version of Vimba
        m_sys_err = m_sys.Startup();                                // Initialize the Vimba API


        // std::stringstream strError;
        if( VmbErrorSuccess == m_sys_err )
        {   
            syson = true;
            m_cam_err = m_sys.GetCameras( m_cameras );            // Fetch all cameras known to Vimba
            if( VmbErrorSuccess == m_cam_err )
            {
                std::cout << "FOUND: " << m_cameras.size() <<" CAMERAS \n\n";
                camera_number = m_cameras.size();
                std::for_each( m_cameras.begin(), m_cameras.end(), PrintCameraInfo );
                
                // m_frames = FramePtrVector()
                m_frames.resize(3);
                
                for (int i = 0 ; i < m_cameras.size() ; i++){
                    m_cam_err = SP_ACCESS(  m_cameras[i] )->Open(VmbAccessModeFull);
                    if (VmbErrorSuccess != m_cam_err){
                        errorLog(m_cam_err, "Could not open camera [error code:" );
                        SP_ACCESS(  m_cameras[i] )->Close();
                        exit(0);
                    }

                    // FeaturePtr pCommandFeature;
                    // if ( VmbErrorSuccess ==  m_cameras[i]->GetFeatureByName( "GVSPAdjustPacketSize", pCommandFeature ))
                    // {
                    //     if ( VmbErrorSuccess == pCommandSP_ACCESS( Feature )->RunCommand() )
                    //     {
                    //         bool bIsCommandDone = false;
                    //         do
                    //         {
                    //             if ( VmbErrorSuccess != pCommandSP_ACCESS( Feature )->IsCommandDone( bIsCommandDone ))
                    //             {
                    //                 break;
                    //             }
                    //         } while ( false == bIsCommandDone );
                    //     }
                    // }

                    setPixelFormat(m_cameras[i]);
                    AdjustBandLimit(m_cameras[i]);
                    enableFrameRate(m_cameras[i]);
                    
                    SP_ACCESS( m_cameras[i] )->Close();
                }

                std::cout<<"camera created"<<std::endl;
                frameallocated = true;
                created = true;
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

        
    }

    void setPixelFormat(CameraPtr cptr){
        FeaturePtr feature;
        SP_ACCESS( cptr )->GetFeatureByName("PixelFormat", feature);
        std::string strValue;
        SP_ACCESS( feature )->GetValue(strValue);
        // std::cout<<"set pixel format to: "<<strValue<<"us"<<std::endl;
        if(std::strcmp(strValue.c_str(), m_frame_format.c_str()) == 0) return;
        else{
        VmbErrorType err = SP_ACCESS( feature )->SetValue(m_frame_format.c_str());
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
            SP_ACCESS( feature )->GetValue(strValue);
            std::cout<<"set pixel format to: "<<strValue<<"us"<<std::endl;
        }
    }

    void enableFrameRate(CameraPtr cptr){
        FeaturePtr feature;
        SP_ACCESS( cptr )->GetFeatureByName("AcquisitionFrameRateEnable", feature);
        SP_ACCESS( feature )->SetValue(false);
        bool enabled;
        SP_ACCESS( feature )->GetValue(enabled);
        std::cout<<"AcquisitionFrameRateEnable: "<<enabled<<std::endl;
    }

    void AdjustFrameRate(CameraPtr cptr){

        FeaturePtr expfeature;
        SP_ACCESS( cptr )->GetFeatureByName("ExposureTime", expfeature);
        double exp;
        SP_ACCESS( expfeature )->GetValue(exp);

        double frame_per_sec = 1000000 / exp - 2;

        FeaturePtr feature;
        SP_ACCESS( cptr )->GetFeatureByName("AcquisitionFrameRate", feature);
        double max_rate, min_rate;
        SP_ACCESS( feature )->GetRange( min_rate, max_rate );

        frame_per_sec = min(max_rate, frame_per_sec);
        VmbErrorType err = SP_ACCESS( feature )->SetValue(frame_per_sec);
        double frame_rate;
        SP_ACCESS( feature )->GetValue(frame_rate);
        std::cout<<"frame_per_sec:"<<frame_per_sec<<"AcquisitionFrameRate: "<<frame_rate<<std::endl;
    }

    void AdjustBandLimit(CameraPtr cptr){
        FeaturePtr bandlimit;
        SP_ACCESS( cptr )->GetFeatureByName("DeviceLinkThroughputLimit", bandlimit);

        long long int max_band, min_band;
        bandlimit->GetRange( min_band, max_band );
        long long int seto = (2 * min_band);
        bandlimit->SetValue(seto);
        long long int after_set;
        bandlimit->GetValue(after_set);
        std::cout<<"band: "<<min_band<<"band: "<<after_set<<std::endl;
    }

    void setExpTime(CameraPtr cptr, double expTime){
        FeaturePtr feature;
        SP_ACCESS( cptr )->GetFeatureByName("ExposureTime", feature );
        double min_exp, max_exp;
        SP_ACCESS( feature )->GetRange( min_exp, max_exp );
        std::cout<<"supprt exp: "<<min_exp<<" max: "<<max_exp<<std::endl;

        SP_ACCESS( feature )->SetValue(expTime);
        double setExpTime;
        SP_ACCESS( feature )->GetValue(setExpTime);
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
            return true;
    }

    void computeHDR(float *image, float *exp){
        for(int i = 0 ; i < m_width * m_height; i++){
            image[i] = image[i] * 1023.0 / (exp[i] / 1000.0);
        }
    }

    bool findSaturated(std::string prefix, double max_exp, double min_exp, double step, int camera_id=0){

        bool over_exp = true;
        float exp = max_exp;

        
        while (exp > min_exp){
            shot(exp, camera_id);
            std::string name = prefix + "_exp_" + std::to_string(exp)+".hdr";
            // over_exp = accumulate_valid_buffer(image_final, time_final, exp);
            readbuffer(name);
            exp -= step;
        }

        return true;
    }



    bool shotHDR(std::string prefix, double start_exp, double scale, int camera_id=0){
            float *image_final = new float[m_width * m_height];
            float *time_final = new float[m_width * m_height];
            
            for ( int i = 0 ; i < m_width * m_height ; i++){
                image_final[i] = 0.0f;
                time_final[i] = 0.0f;
            }

            bool over_exp = true;
            float exp = start_exp;

            while (over_exp){
                shot(exp, camera_id);
                std::string name = prefix + "_exp_" + std::to_string(exp)+".hdr";
                over_exp = accumulate_valid_buffer(image_final, time_final, exp);
                exp *= scale;
            }


            computeHDR(image_final, time_final);
            stbi_write_hdr((prefix+"_mono_hdr.hdr").c_str(), m_width, m_height, 1, image_final);
            // printf("write to hdr");
            delete image_final;
            delete time_final;

            return true;
    }

    bool shot(double exp=40000, int id=0){
        std::cout<<"start taking photo of camera"<<id<<" / "<<camera_number<<std::endl;
        CameraPtr camera_ptr = m_cameras[id];
        VmbErrorType err = SP_ACCESS(camera_ptr)->Open( VmbAccessModeFull );
        if ( VmbErrorSuccess != err )
        {
            errorLog(err, "Could not get camera id [error code:" );
            err = SP_ACCESS(camera_ptr)->Close();
            errorLog(err, "Could not close camera [error code: " );
            return false;
        }

        AdjustBandLimit(camera_ptr);
        setExpTime(camera_ptr, exp);
        
        if (m_frames[0] != nullptr){
            VmbFrameStatusType eReceiveStatus;
            err = m_frames[0]->GetReceiveStatus( eReceiveStatus );
            std::cout<<" recieve status: "<<eReceiveStatus<<std::endl;
        }

        VmbInt32_t timeoutvalue = ( m_frames.size() + 1) * exp / 1000;
        VmbUint32_t image_count = 0;
        err = SP_ACCESS(camera_ptr)->AcquireMultipleImages(m_frames, timeoutvalue, image_count, FrameAllocation_AnnounceFrame);
        
        if ( VmbErrorSuccess != err ){
            std::cout<<" number of image captured "<<image_count<<std::endl;
            errorLog(err, " [ERROR] timeout! can not capture image " );
        }

        SP_ACCESS(camera_ptr)->Close();
        if( image_count > 0)
            return true;
        else return false;
    }

    void getImageSize(){
        FramePtr fptr = m_frames[0];
        fptr->GetWidth(m_width);
        fptr->GetHeight(m_height);
        fptr->GetImageSize(m_image_buffer_size);
        // std::cout<<"image size in bytes: "<<m_image_buffer_size<<" image width x height "<<m_width*m_height<<" bit per pixel"<<m_image_buffer_size * 8 / (m_width * m_height)<<std::endl;
    }

    void * getImage(int id){
        FramePtr fptr = m_frames[0];
        VmbUchar_t* buffer_ptr;
        fptr->GetImage(buffer_ptr);
        return buffer_ptr;
    }

    int flush(std::string filename, int id=0){
        return readbuffer(filename);
    }

    bool readbuffer(std::string filename){
        FramePtr fptr = m_frames[0];

        VmbFrameStatusType eReceiveStatus;
        VmbErrorType err = fptr->GetReceiveStatus( eReceiveStatus );
        
        VmbUchar_t* pBuffer;
        fptr->GetImage(pBuffer);

        unsigned int buffersizeInByte;
        fptr->GetImageSize(buffersizeInByte);

        std::cout<<"buffer size in byte"<<buffersizeInByte<<std::endl;

        float * pImage;
                
        VmbUint32_t width, height;
        fptr->GetWidth(width);
        fptr->GetHeight(height);

        pImage = new float[width * height];

        if(m_frame_format == "BayerGR10"){
            unsigned int pixel = buffersizeInByte / 2;

            float saturated = 1023.0;
            // std::int64_t mask = 1023;
            for ( int i = 0 ; i < pixel ; i++){
                float pixelValue;
                std::memcpy(&pixelValue, pBuffer+(i*2), 2); // OK
                float px = float(pixelValue) / saturated;
                pImage[i] = px;
            }

        }else if(m_frame_format == "BayerGR10p"){

            unsigned int packet_number = buffersizeInByte / 5;

            std::int64_t mask_3 = 1023;
            std::int64_t mask_0 = (mask_3<<30);
            std::int64_t mask_1 = (mask_3<<20);
            std::int64_t mask_2 = (mask_3<<10);

            float saturated = 1023.0;
            
            for ( int i = 0 ; i < packet_number ; i++){             
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

                pImage[i * 4 + 3] = float(G_4);
                pImage[i * 4 + 2] = float(R_4);
                pImage[i * 4 + 1] = float(B_4);
                pImage[i * 4 + 0] = float(G2_4);
            }
        }

        bool write_err = stbi_write_hdr(filename.c_str(), width, height, 1, pImage);
        delete pImage;
        return write_err;
    }

    void getbuffer(float *dtr){
        FramePtr fptr = m_frames[0];

        VmbFrameStatusType eReceiveStatus;
        VmbErrorType err = fptr->GetReceiveStatus( eReceiveStatus );
        
        VmbUchar_t* pBuffer;
        fptr->GetImage(pBuffer);

        unsigned int buffersizeInByte;
        fptr->GetImageSize(buffersizeInByte);

        if(m_frame_format == "BayerGR10"){
            unsigned int pixel = buffersizeInByte / 2;
            // std::int64_t mask = 1023;
            for ( int i = 0 ; i < pixel ; i++){
                float pixelValue;
                std::memcpy(&pixelValue, pBuffer+(i*2), 2); // OK
                dtr[i] = pixelValue;
            }

        }else if(m_frame_format == "BayerGR10p"){

            unsigned int packet_number = buffersizeInByte / 5;

            std::int64_t mask_3 = 1023;
            std::int64_t mask_0 = (mask_3<<30);
            std::int64_t mask_1 = (mask_3<<20);
            std::int64_t mask_2 = (mask_3<<10);

            float saturated = 1023.0;
            
            for ( int i = 0 ; i < packet_number ; i++){             
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
    }


    bool accumulate_valid_buffer(float *image_acc, float *exp_total, float exp){
        FramePtr fptr = m_frames[0];

        VmbFrameStatusType eReceiveStatus;
        VmbErrorType err = fptr->GetReceiveStatus( eReceiveStatus );
        
        VmbUchar_t* pBuffer;
        fptr->GetImage(pBuffer);

        unsigned int buffersizeInByte;
        fptr->GetImageSize(buffersizeInByte);
        bool over_exp = false;

        if(m_frame_format == "BayerGR10"){
            unsigned int pixel = buffersizeInByte / 2;
            float saturated = 1023.0;
            for ( int i = 0 ; i < pixel ; i++){
                float pixelValue;
                std::memcpy(&pixelValue, pBuffer+(i*2), 2); // OK
                float px = pixelValue / saturated;
                over_exp |= (px >= 1.0);
                image_acc[i] += (px>=1.0? 0.0: px);
                exp_total[i] += (px>=1.0? 0.0: float(exp));
            }

        }else if(m_frame_format == "BayerGR10p"){
            unsigned int packet_number = buffersizeInByte / 5;
            std::int64_t mask_3 = 1023;
            std::int64_t mask_0 = (mask_3<<30);
            std::int64_t mask_1 = (mask_3<<20);
            std::int64_t mask_2 = (mask_3<<10);

            float saturated = 1023.0;
            for ( int i = 0 ; i < packet_number ; i++){             
                std::int64_t packet;
                std::memcpy(&packet, pBuffer+(i*5), 5); // OK
                
                std::int64_t G = ((packet & mask_0) >> 30);
                std::int64_t R = ((packet & mask_1) >> 20);
                std::int64_t B = ((packet & mask_2) >> 10);
                std::int64_t G2 = (packet & mask_3);

                float saturated = 1023.0 - 1;

                float G_4 = float(G) / saturated;
                float R_4 = float(R) / saturated;
                float B_4 = float(B) / saturated;
                float G2_4 = int32_t(G2) / saturated;

                over_exp |= (G_4 >= 1.0 | R_4 >= 1.0 | B_4 >= 1.0 | G2_4 >= 1.0);

                image_acc[i * 4 + 3] += (G_4 >= 1.0f ? 0.0f: float(G_4));
                image_acc[i * 4 + 2] += (R_4 >= 1.0f ? 0.0f: float(R_4));
                image_acc[i * 4 + 1] += (B_4 >= 1.0f ? 0.0f: float(B_4));
                image_acc[i * 4 ] += (G2_4 >= 1.0f ? 0.0f: float(G2_4));

                exp_total[i * 4 + 3] += (G_4 >= 1.0f ? 0.0f: float(exp));
                exp_total[i * 4 + 2] += (R_4 >= 1.0f ? 0.0f: float(exp));
                exp_total[i * 4 + 1] += (B_4 >= 1.0f ? 0.0f: float(exp));
                exp_total[i * 4] += (G2_4 >= 1.0f ? 0.0f: float(exp));

            }
        }

        return over_exp;
    }

    ~Camera2(){
        // if (frameallocated){
        //     free(m_frames);
        //     frameallocated = false;
        // }

        if (syson){
            VmbErrorType err = m_sys.Shutdown();
            errorLog(err, "Could not shutdown Vimba [error code: " );
            syson = false;
        }
    }

    VmbErrorType m_sys_err;
    VmbErrorType m_cam_err;

    FramePtrVector m_frames;
    bool syson = false;
    bool frameallocated = false;
    bool created = false;
    unsigned int m_width;
    unsigned int m_height;
    int camera_number = 0;
    unsigned int m_image_buffer_size;
    std::string m_frame_format = "BayerGR10p";

    // Mat black;

    VimbaSystem& m_sys = VimbaSystem::GetInstance();  
    CameraPtrVector m_cameras;                           // A vector of std::shared_ptr<AVT::VmbAPI::Camera> objects
};

// }
// }