#include<iostream>
#include<string>
#include "Serial.h"

#include <boost/filesystem.hpp>
// #include ""

using namespace std;

// void photomode1(Camera *camera){
//     // ;
//     // printf("sleep\n");
    
// }

int main(int argc, char **argv){

    int id = 0;
    if(argc > 1){
        id = std::stoi(std::string(argv[1]));
    }

    Serial arduino("/dev/ttyACM0");
    char ret;
    arduino.readChar(&ret);
    if(ret=='R');
    arduino.turnOff(id);

    return 0;
}
