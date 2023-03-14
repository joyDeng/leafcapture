#include<iostream>
#include<string>
#include "Serial.h"
#include "Window.h"
#include "Capture.h"
#include "Camera.h"
#include "Camera2.h"

#include <boost/filesystem.hpp>

using namespace std;


int main(int argc, char **argv){
    std::string checkboard_id="0";
    if(argc > 1){
        checkboard_id = std::string(argv[1]);
    }

    std::string fileprefix="calibrate_2";
    std::string DATA_ROOT = "/home/dx/Research/leaf/data/";
    std::string data_path = DATA_ROOT + fileprefix;
    boost::filesystem::create_directories(data_path);

    Serial arduino("/dev/ttyACM0");
    char ret;
    arduino.readChar(&ret);
    if(ret=='R');
    arduino.turnOn(3);

    ImageWindow *window;
    window = new ImageWindow();
    window->launchWindow();
    
    double start_exp = 20000;
    double scale = 0.5f;
    // int numImage = 5;
    arduino.turnOff(3);

    for (int lid = 0 ; lid < 7 ; lid++){
        arduino.turnOn(lid);
        for (int cid = 0 ; cid < 3 ; cid++){
            std::this_thread::sleep_for (std::chrono::seconds(1));
            window->m_camera->shotHDR(data_path+"/checker_" + checkboard_id + "_c"+std::to_string(cid)+"_l"+std::to_string(lid), start_exp, scale, cid);   
            // cid = cid-1;
        }
        arduino.turnOff(lid);
    }

    delete window;

    return 0;
}
