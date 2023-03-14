#include<iostream>
#include<string>
#include "Serial.h"
#include "Window.h"
#include "Capture.h"
#include "Camera.h"
#include "Camera2.h"

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

    // exit(0);
    // std::string fileprefix="test_light_";
    int light = 3;
    if(argc > 1){
        light = std::stoi(std::string(argv[1]));
    }


    Serial arduino("/dev/ttyACM0");
    char ret;
    arduino.readChar(&ret);
    if(ret=='R');
    arduino.turnOn(light);


    ImageWindow *window;
    window = new ImageWindow();


    window->launchWindow();
    
    arduino.turnOff(light);

    // // take a black image
    // std::this_thread::sleep_for (std::chrono::seconds(1));
    // window->m_camera->shot();
    // std::string name=fileprefix+"_black.png";
    // window->m_camera->flush(name.c_str());

    
    // // std::this_thread::sleep_for (std::chrono::seconds(2));
    // for(int i = 0 ; i < 7 ; i++){
    //     arduino.turnOn(i);
    //     std::this_thread::sleep_for (std::chrono::seconds(1));
    //     window->m_camera->shot();
        // name=fileprefix+std::to_string(i)+".png";
        // std::this_thread::sleep_for (std::chrono::seconds(2));
        // printf("shot token\n");
        // if(!window->m_camera->flush(name.c_str())){
        //     printf("\n failed to write image \n");
        // } else printf("written_\n");

    //     arduino.turnOff(i);
    // }

    return 0;
}
