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

#define numLight 11

class Serial{
    bool connected=false;
    int id;
    // char lightids[7] = {'00', '01', '02', '03', '04', '05', '06', '07', '08', '09', '10', '11'};
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
        std::cout<<" serial initialized "<<std::endl;
    };

    ~Serial(){
        // if(m_port != nullptr) delete m_port;
                // if (m_port->is_open()){
            // std::cout<<" now close the port "<<std::endl;
            // m_port->close();
            // std::cout<<" close port "<<std::endl;
            // free(m_port);
        // }
        std::cout<<"deconstruct serial port"<<std::endl;
    }

    void sendChar(char *a) {
        printf(" sending cmd %c \n", *a);
        write(*m_port, buffer(a, 3));
    }

    void readChar(char *a) {
        read(*m_port, buffer(a, 1));
    }

    void turnOn(int id){
        int decimal = id / 10;
        int digital = id % 10;
        char cmd[3] = {(char)(decimal + '0'), (char)(digital + '0'), 'O'};
        bool turned_on = false;
        while(!turned_on){
            sendChar(cmd);
            char ret;
            readChar(&ret);
            if(ret == 'D') turned_on = true;
        }
        assert(turned_on==true);
    }

    void turnOff(int id){
        int decimal = id / 10;
        int digital = id % 10;
        char cmd[3] = {(char)(decimal + '0'), (char)(digital + '0'), 'F'};
        bool turned_off = false;        
        while(!turned_off){
            sendChar(cmd);
            char ret;
            readChar(&ret);
            if(ret == 'D') turned_off = true;
        }

        assert(turned_off==true);
        // if (ret == 'D')
        //     printf("light %d turned Off \n" , id);
        // else
        //     printf("light %d not turned off \n", id);
    }

    bool isConnected(){return connected;}
};