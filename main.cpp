#include<iostream>
#include<string>
#include "Serial.h"
#include "Camera.h"
#include "Window.h"
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
    
    std::string fileprefix="test_light_";
    if(argc > 1){
        fileprefix = std::string(argv[1]);
    }


    Serial arduino("/dev/tty.usbmodem14401");
    char ret;
    arduino.readChar(&ret);
    if(ret=='R');
    arduino.turnOn(3);


    ImageWindow *window;
    window = new ImageWindow();



    // int start = 1;
        // launch camera

    // auto fc = [](Camera *camera, int *cc) {
    //     printf("threading 1");
    //     Camera::view(camera, cc);
    // };

    // std::cout<<"creating new thread"<<std::endl;

    // std::thread th1(fc, window->m_camera, &start);

    // std::cout<<"starting new thread"<<std::endl;

    // // th1.start();

    // std::cout<<"start to render"<<std::endl;

    window->launchWindow();


    // Serial arduino("/dev/tty.usbmodem14401");
    arduino.turnOff(3);

    // take a black image
    std::this_thread::sleep_for (std::chrono::seconds(1));
    window->m_camera->shot();
    std::string name=fileprefix+"_black.png";
    window->m_camera->flush(name.c_str());

    
    // std::this_thread::sleep_for (std::chrono::seconds(2));
    for(int i = 0 ; i < 7 ; i++){
        arduino.turnOn(i);
        std::this_thread::sleep_for (std::chrono::seconds(1));
        window->m_camera->shot();
        name=fileprefix+std::to_string(i)+".png";
        // std::this_thread::sleep_for (std::chrono::seconds(2));
        printf("shot token\n");
        if(!window->m_camera->flush(name.c_str())){
            printf("\n failed to write image \n");
        } else printf("written_\n");

        arduino.turnOff(i);
    }

    // char ret;
    // arduino.readChar(&ret);
    // printf("ret %c \n ", ret);
    // std::cout<<"receiving char: "<<ret<<std::endl;

    // std::this_thread::sleep_for (std::chrono::seconds(2));





    // arduino.sendChar('1');
    // std::this_thread::sleep_for (std::chrono::seconds(2));

    // window->m_camera->shot();
    // std::this_thread::sleep_for (std::chrono::seconds(2));
    // printf("shot token\n");
    // if(!window->m_camera->flush("test_light_back.png")){
    //     printf("\n failed to write image \n");
    // }
    // printf("written_ 1\n");


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
