#include<iostream>
#include<string>
#include "Capture.h"
#include "Camera.h"
#include "AllivedCameras.h"
#include <boost/filesystem.hpp>
#include <ctime>   
// #include "Serial.h"

using namespace std;

#include <chrono>
#include <thread>
using namespace std::chrono;

#define MAX_EXP 2000000

int measure_all(string data_path){
    CameraC *m_camera = new CameraC();
    float start_exp = MAX_EXP;
    if(m_camera->created){
        for(int camera_id = 0; camera_id < m_camera->camera_number ; camera_id++){
            bool suc = m_camera->shotHDR(data_path + "/c"+std::to_string(camera_id), start_exp, 0.5, camera_id);
            if(suc){
                std::cout<<"caputered: camera_id "<<camera_id<<std::endl;
            }else{
                return 0;
            }
        }
    } else {
        std::cout<<"failed to create cameras"<<std::endl;
        return 0;
    }
    return 1;
}

int main(int argc, char **argv){
    std::string fileprefix="measured_at_";
    int operation = 0;
    if(argc > 1){
        fileprefix = std::string(argv[1]);
    }

    std::time_t now = time(0);
    char *dt = ctime(&now);
    std::string time_stamp = dt;

    std::string DATA_ROOT = "/home/xd93/data/";
    std::string data_path = DATA_ROOT + fileprefix + time_stamp;
    
    std::cout<<"now: "<<dt<<" time stamp"<<time_stamp;
    boost::filesystem::create_directories(data_path);
    int suc = measure_all(data_path);
    return suc;
}
