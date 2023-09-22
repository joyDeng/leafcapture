#include<iostream>
#include<string>
#include "Serial.h"

#include <boost/filesystem.hpp>
using namespace std;
#define NUM_LIGHT 11

bool testalllight(Serial &myport, int num_light){
    for(int i = 0 ; i < num_light ;i++){
        myport.turnOn(i);
        sleep(1);
        myport.turnOff(i);
    }
    return true;
}

int main(int argc, char **argv){

    int id = 0;
    std::string cm = "f";
    if(argc > 1){
        id = std::stoi(std::string(argv[1]));
    }
    if(argc > 2){
        cm = std::string(argv[2]);
    }

    Serial arduino("/dev/ttyACM0");
    char ret;
    arduino.readChar(&ret);
    printf("turn id = %d, %c, %s", id, ret, cm);
    if(ret=='R');

    if(cm=="o") {
        printf("turn id = %d, %c, on \n", id, ret, cm);
        arduino.turnOn(id);
    }else if(cm == "f"){
        printf("turn id = %d, %c, off \n", id, ret, cm);
        arduino.turnOff(id);
    }else{
        for(int i = 0 ; i < NUM_LIGHT ; i++)
            arduino.turnOff(i);
    }
    return 0;
}
