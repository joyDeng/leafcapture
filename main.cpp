#include<iostream>
#include<string>
#include "Serial.h"
#include "Window.h"
#include "Capture.h"
#include "Camera.h"
#include "Camera2.h"

#include <boost/filesystem.hpp>

// #include ""

using namespace std;

// void photomode1(Camera *camera){
//     // ;
//     // printf("sleep\n");
    
// }

int main(int argc, char **argv){
    // std::cout<<"hello world"<<std::endl;
    // Serial arduino("/dev/tty.usbmodem14101");
    // std::cout<<"create arduino device, its connection is: "<<arduino.isConnected()<<std::endl;
    // Camera2 test2;
    // Camera1 test1;

    // test.shot();
    // test.develop();

    // test read 

    // exit(0);
    std::string fileprefix="test_light_";
    if(argc > 1){
        fileprefix = std::string(argv[1]);
    }

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
    
    double start_exp = 400000;
    double scale = 0.5f;

    arduino.turnOff(3);
    
    for(int i = 0 ; i < 7 ; i++){
        arduino.turnOn(i);
        for (int cid = window->m_camera->camera_number-1; cid >= 0  ; cid--){
            std::this_thread::sleep_for (std::chrono::seconds(1));
            window->m_camera->shotHDR(data_path+"/c"+std::to_string(cid)+"_l"+std::to_string(i), start_exp, scale, cid);
        }
        arduino.turnOff(i);
    }

    return 0;
}
