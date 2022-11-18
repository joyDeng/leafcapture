#pragma once
#include <iostream>
#include <string>
#include <boost/asio.hpp>
#include <boost/asio/serial_port.hpp>
#include <boost/asio/streambuf.hpp>
#include <boost/asio/serial_port_base.hpp>

// #include <boost/asio/basic_serial_port.hpp>

using namespace std;
using namespace boost::asio;

#define numLight 7



class Serial{
    bool connected=false;
    int id;
    char lightids[7] = {'0', '1', '2', '4', '5', '6', '7'};
    // io_service m_io;
    string port_name;
    serial_port *m_port;
    public:
    Serial(string name):port_name(name){
        io_service io;
        m_port = new serial_port(io, name);
        m_port->set_option(serial_port_base::baud_rate(9600));
        if (m_port->is_open()) {
            std::cout<<" open the port "<<std::endl;
            connected = true;
        }else {
            std::cout<<" the port is not open "<<std::endl;
            connected = false;
        }
    };

    ~Serial(){
        if (m_port->is_open()){
            std::cout<<" now close the port "<<std::endl;
            m_port->close();
        }
    }

    void sendChar(char *a) {
        printf("sending cmd %c", *a);
        // const_buffer buf(&a, 1);
        // buf(&count, 4);
        write(*m_port, buffer(a, 2));
    }

    void readChar(char *a) {
        // for (int i=0; i<2; i++){
            // char c;
        read(*m_port, buffer(a, 1));
            // ret += c;
        // }
    }

    void turnOn(int id){
        char cmd[2] = {lightids[id % numLight], 'O'};
        sendChar(cmd);
        char ret;
        readChar(&ret);
        if (ret == 'D')
            printf("light %d turned ON \n" , id);
        else
            printf("light %d not turned on \n", id);
    }

    void turnOff(int id){
        char cmd[2] = {lightids[id % numLight], 'F'};
        sendChar(cmd);
        char ret;
        readChar(&ret);
        if (ret == 'D')
            printf("light %d turned Off \n" , id);
        else
            printf("light %d not turned off \n", id);
    }

    bool isConnected(){return connected;}
};