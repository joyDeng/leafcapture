#pragma once
#include<iostream>
#include <string>
#include <dc1394/dc1394.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <inttypes.h>
#include <cstring>
#include "Capture.h"

// #define IMAGE_FILE_NAME "image.png"

class Camera1 : public CaptureDevice{
    // CameraType m_type=CameraType::CCHAMELON;
    // ImageWindow m_viewer;
    FILE* imagefile;
    dc1394camera_t *camera;
    dc1394featureset_t features;
    dc1394framerates_t framerates;
    dc1394video_modes_t video_modes;
    dc1394framerate_t framerate = DC1394_FRAMERATE_MAX;
    dc1394video_mode_t video_mode = DC1394_VIDEO_MODE_MIN;
    dc1394color_coding_t coding;


    public:
    dc1394_t * m_d;
    dc1394camera_list_t *list;
    dc1394video_frame_t *new_frame;
    dc1394video_frame_t *temp_frame;

    dc1394error_t err;
    bool created=false;
    // int cam_id;

    void cleanup_and_exit(dc1394camera_t *camera){
        dc1394_video_set_transmission(camera, DC1394_OFF);
        dc1394_capture_stop(camera);
        dc1394_camera_free(camera);
        created = false;
        exit(1);
    }

    public:


    Camera1(){
        m_d = dc1394_new();
        err = dc1394_camera_enumerate(m_d, &list);
        DC1394_ERR(err, "Failed to enumerate camera");
        if (list->num == 0){
            dc1394_log_error("No camera found");
            return;
        }
        int cam_ind;
    
        // for (cam_ind = 0 ; cam_ind < list->num ; cam_ind++){
        //     char cam_id[17];
        //     sprintf(cam_id, "%" PRIx64, list->ids[cam_ind].guid);
        //     // if (strlen(cam_id) >= strlen(id_tail) &&
        //     std::cout<<"cam_id"<<cam_id<<std::endl;
        // }
        cam_ind = 0;

        camera = dc1394_camera_new(m_d, list->ids[cam_ind].guid);
        if (!camera){
            dc1394_log_error("failed to initialize camera with guid %" PRIx64, list->ids[0].guid);
        }
        dc1394_camera_free_list (list);

        printf("Using camera with GUID %" PRIx64 "\n", camera->guid);
        

        err=dc1394_video_get_supported_modes(camera, &video_modes);
        DC1394_ERR_CLN(err,cleanup_and_exit(camera),"Can't get video modes");

        int i;
        // select highest res mode:
        printf("support %d modes \n", video_modes.num);
        for ( i=video_modes.num-1;i>=0;i--) {
            // if (!dc1394_is_video_mode_scalable(video_modes.modes[i])) {
                dc1394_get_color_coding_from_video_mode(camera,video_modes.modes[i], &coding);
                std::cout<<"mode:"<<coding<<std::endl;
                if (coding==361) { // DC1394_COLOR_CODING_MONO8
                    // std::cout<<"mode:"<<coding<<std::endl;
                    video_mode=video_modes.modes[i];
                    break;
                }
            // }
        }
        // video_mode = video_modes.modes[video_modes.num-1];
        if (i < 0) {
            dc1394_log_error("Could not get a valid mode");
            cleanup_and_exit(camera);
        }

        // exit(0);

        err=dc1394_get_color_coding_from_video_mode(camera, video_mode,&coding);
        DC1394_ERR_CLN(err,cleanup_and_exit(camera),"Could not get color coding");

        // get highest framerate
        // err=dc1394_video_get_supported_framerates(camera,video_mode,&framerates);
        // DC1394_ERR_CLN(err,cleanup_and_exit(camera),"Could not get framrates");
        // framerate=framerates.framerates[framerates.num-1];
        created = true;
        std::cout<<"created"<<std::endl;
    }

    /*should excute this in parrallel*/
    // static void view(Camera1 *camera, int * stop){
    //     while((*stop)!=0){
    //         // printf("iteration %d \n");
    //         camera->shot();
    //     }
    // }

    void setShutterSpeed(int shutter_speed){
        err=dc1394_video_set_framerate(camera, framerate);
        DC1394_ERR_CLN(err,cleanup_and_exit(camera),"Could not set framerate");

        err = dc1394_feature_set_value (camera, DC1394_FEATURE_SHUTTER, shutter_speed);
        DC1394_ERR_CLN(err,dc1394_camera_free (camera),"cannot set shutter");
       std::cout<<"I: shutter is "<<shutter_speed<<" \n"<<std::endl;
    }

    void setupCapture(int shutter_speed){
    /*-----------------------------------------------------------------------
     *  setup capture
     *-----------------------------------------------------------------------*/

        err=dc1394_video_set_iso_speed(camera, DC1394_ISO_SPEED_400);
        DC1394_ERR_CLN(err,cleanup_and_exit(camera),"Could not set iso speed");

        // err=dc1394_video_set_exposure(camera,)

        err=dc1394_video_set_mode(camera, video_mode);
        DC1394_ERR_CLN(err,cleanup_and_exit(camera),"Could not set video mode");

        // err=dc1394_video_set_framerate(camera, framerate);
        // DC1394_ERR_CLN(err,cleanup_and_exit(camera),"Could not set framerate");

        // err = dc1394_feature_set_value (camera, DC1394_FEATURE_SHUTTER, shutter_speed);
        // DC1394_ERR_CLN(err,dc1394_camera_free (camera),"cannot set shutter");
        // printf ("I: shutter is 50\n");

        err = dc1394_feature_set_value(camera, DC1394_FEATURE_BRIGHTNESS, 10);
        DC1394_ERR_CLN(err,dc1394_camera_free (camera),"cannot set temperature");
        printf ("I: brightness is 10\n");
        
        uint32_t ubvalue;
        uint32_t vrvalue;
        dc1394_feature_whitebalance_get_value(camera, &ubvalue, &vrvalue);
        printf("ubvalue = %d, vrvalue = %d", ubvalue, vrvalue);

        dc1394_feature_whitebalance_set_value(camera, 780, vrvalue);

        // DC1394_FEATURE_FOCUS
        // err = dc1394_feature_set_value (camera, DC1394_FEATURE_FOCUS, 15);
        // DC1394_ERR_CLN(err,dc1394_camera_free (camera),"cannot set focus");
        // printf ("I: focus is 20\n");

        // err=dc1394_get_adv_control_register(camera,0x390U, &value);
        // DC1394_ERR(err,"Could not get AVT autofocus AOI");

        /*-----------------------------------------------------------------------
        *  report camera's features
        *-----------------------------------------------------------------------*/
        err=dc1394_feature_get_all(camera,&features);
        if (err!=DC1394_SUCCESS) {
            dc1394_log_warning("Could not get feature set");
        }
        else {
            // dc1394_feature_print_all(&features, stdout);
        }
    }

    void shot(){
        err=dc1394_capture_setup(camera,4, DC1394_CAPTURE_FLAGS_DEFAULT);
        DC1394_ERR_CLN(err,cleanup_and_exit(camera),"Could not setup camera-\nmake sure that the video mode and framerate are\nsupported by your camera");

        // printf("start transmission\n");
        /*-----------------------------------------------------------------------
        *  have the camera start sending us data
        *-----------------------------------------------------------------------*/
        err=dc1394_video_set_transmission(camera, DC1394_ON);
        DC1394_ERR_CLN(err,cleanup_and_exit(camera),"Could not start camera iso transmission");
        


        // printf("start dequeue\n");
        /*-----------------------------------------------------------------------
        *  capture one frame
        *-----------------------------------------------------------------------*/
        err=dc1394_capture_dequeue(camera, DC1394_CAPTURE_POLICY_WAIT, &frame);
        DC1394_ERR_CLN(err,cleanup_and_exit(camera),"Could not capture a frame");

        /*-----------------------------------------------------------------------
        *  stop data transmission
        *-----------------------------------------------------------------------*/
        // printf("start end transmission\n");
        err=dc1394_video_set_transmission(camera,DC1394_OFF);
        DC1394_ERR_CLN(err,cleanup_and_exit(camera),"Could not stop the camera");
        // printf("end taking\n");
        /*-----------------------------------------------------------------------
        *  save image as 'Image.pgm'
        *-----------------------------------------------------------------------*/
   

        dc1394_get_image_size_from_video_mode(camera, video_mode, &width, &height);
        // printf("got image size\n");
        if(temp_frame == nullptr) temp_frame = (dc1394video_frame_t *) calloc(1,sizeof(dc1394video_frame_t));
        // printf("new frame\n");
        temp_frame->color_coding=DC1394_COLOR_CODING_RAW8;
        memcpy(temp_frame, frame, sizeof(dc1394video_frame_t));

        err=dc1394_capture_enqueue(camera, frame);
        DC1394_ERR_CLN(err,cleanup_and_exit(camera),"Could not clean up frames");

        // err=dc1394_capture_dequeue(camera, DC1394_CAPTURE_POLICY_POLL, &frame);

        // DC1394_ERR_CLN(err,cleanup_and_exit(camera),"Could not capture a frame");
        // while(err){
        //     err=dc1394_capture_dequeue(camera, DC1394_CAPTURE_POLICY_POLL, &frame);
        //     printf("flushing");
        //     if (frame == NULL) return;
        // }

        if(new_frame == nullptr) new_frame = (dc1394video_frame_t *) calloc(1,sizeof(dc1394video_frame_t));
        new_frame->color_coding=DC1394_COLOR_CODING_RGB8;
        // printf("convert \n");
        // dc1394_convert_frames(frame, new_frame);
        dc1394_debayer_frames(temp_frame, new_frame, DC1394_BAYER_METHOD_BILINEAR);

        // printf("start enqueue\n");
        

        // fprintf(imagefile,"P5\n%u %u 255\n", width, height);
        // fwrite(frame->image, 1, height*width, imagefile);
        // fclose(imagefile);
        // printf("wrote: " IMAGE_FILE_NAME "\n");

        dc1394_video_set_transmission(camera, DC1394_OFF);
        dc1394_capture_stop(camera);
        /*-----------------------------------------------------------------------
        *  close camera
        *-----------------------------------------------------------------------*/


        // return;
    };

    bool flush(std::string filename){
        imagefile=fopen(filename.c_str(), "wb");

        if( imagefile == NULL) {
            printf( "Can't create %s", filename.c_str());
            // cleanup_and_exit(camera);
            return false;
        }

        fprintf(imagefile,"P6\n%u %u\n255\n", width, height);
        fwrite((const char *)new_frame->image, 1, height*width*3, imagefile);
        fclose(imagefile);
        printf("wrote: %s \n", filename.c_str());
        return true;
    }


    ~Camera1(){
        if(camera != nullptr){
            dc1394_video_set_transmission(camera, DC1394_OFF);
            dc1394_capture_stop(camera);
            dc1394_camera_free(camera);
            dc1394_free (m_d);
        }

        if (new_frame != nullptr){
            if(new_frame->image != nullptr){
                free(new_frame->image);
                free(new_frame);
                free(temp_frame->image);
                free(temp_frame);
            }
        }
    }

    uint32_t width, height;
    dc1394video_frame_t *frame;
    // CameraType getType(){return m_type;}
};