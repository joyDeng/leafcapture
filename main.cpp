#include<iostream>
#include<string>
#include "Serial.h"
#include "Camera.h"
#include "Window.h"

using namespace std;

int main(){
    std::cout<<"hello world"<<std::endl;
    // Serial arduino("/dev/tty.usbmodem14501");
    // std::cout<<"create arduino device, its connection is: "<<arduino.isConnected()<<std::endl;

    ImageWindow window;
    window.launchWindow();
    // while(1){
    //     std::cout<<"please enter light you want to trigger, 0, 1, 2, press e to escape" <<std::endl;
    //     char light;
    //     std::cin>>light;
    //     if (light == 'e') 
    //         break;
    //     if (light == '1' || light == '0' || light == '2'){
    //         std::cout<<"turning on light 0"<<std::endl;
    //         arduino.sendChar(light);
    //     }else{
    //         std::cout<<"invalid input, please enter again"<<std::endl;
    //     }
    // }

    

    // std::cout<<"turning on light 1"<<std::endl;
    // arduino.sendChar('1');
    // std::cout<<"checking camera"<<std::endl;
    // arduino.sendChar('2');
    // Camera cameras;
    // std::cout<<"create usb camera, its type is: "<<camera0.getType()<<std::endl;
    return 0;
}
