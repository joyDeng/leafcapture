#include<iostream>
#include<string>
#include "Serial.h"
// #include "Window.h"
#include "Capture.h"
#include "Camera.h"
// #include "Camera2.h"
#include "AllivedCameras.h"

#include <boost/filesystem.hpp>

// #include ""

using namespace std;

#include <chrono>
#include <thread>
using namespace std::chrono;

#define NUM_LIGHT 11
// void photomode1(Camera *camera){
//     // ;
//     // printf("sleep\n");
    
// }
int measure_camera_light(string data_path, int cid, int lid){

    Serial arduino("/dev/ttyACM0");
    char ret;
    arduino.readChar(&ret);
    if(ret=='R');

    CameraC *m_camera = new CameraC();
    float start_exp = 1000000;
    if(m_camera->created){
        arduino.turnOn(lid);
        sleep(1);
        bool suc = m_camera->shotHDR(data_path + "/c"+std::to_string(cid)+"_l"+std::to_string(lid), start_exp, 0.5, cid);
        if(suc){
            std::cout<<"caputered: light "<<lid<<" cid "<<cid<<std::endl;
        }else{
            return 0;
        }
        arduino.turnOff(lid);
    } else {
        std::cout<<"failed to create cameras"<<std::endl;
        return 0;
    }
    return 1;
}

int measure_all(string data_path){

    Serial arduino("/dev/ttyACM0");
    char ret;
    arduino.readChar(&ret);
    if(ret=='R');

    CameraC *m_camera = new CameraC();
    float start_exp = 1000000;
    if(m_camera->created){
        for (int light_id = 0; light_id < NUM_LIGHT ; light_id++){
            arduino.turnOn(light_id);
            sleep(1);
            for(int camera_id = 0; camera_id < m_camera->camera_number ; camera_id++){
                bool suc = m_camera->shotHDR(data_path + "/c"+std::to_string(camera_id)+"_l"+std::to_string(light_id), start_exp, 0.5, camera_id);
                if(suc){
                    std::cout<<"caputered: light "<<light_id<<" camera_id "<<camera_id<<std::endl;
                }else{
                    return 0;
                }
            }
            arduino.turnOff(light_id);
        }
    } else {
        std::cout<<"failed to create cameras"<<std::endl;
        return 0;
    }
    return 1;
}

int main(int argc, char **argv){
    std::string fileprefix="test_light_";
    int operation = 0;
    if(argc > 1){
        fileprefix = std::string(argv[1]);
    }

    if (argc > 2){
        operation = 1;
    }

    std::string DATA_ROOT = "/home/dx/Research/leaf/data/";
    std::string data_path = DATA_ROOT + fileprefix;
    boost::filesystem::create_directories(data_path);
    if(operation > 0){
        int camera_id = std::stoi(std::string(argv[2]));
        int light_id = std::stoi(std::string(argv[3]));
        std::cout<<"start to measure light"<<light_id<<" from camera "<<camera_id<<std::endl;
        int suc = measure_camera_light(data_path, camera_id, light_id);
        return suc;
    }else{
        int suc = measure_all(data_path);
        return suc;
    }
}
