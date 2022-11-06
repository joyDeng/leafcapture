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

class Serial{
    bool connected=false;
    int id;
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

    void sendChar(char a) {
        const_buffer buf(&a, 1);
        m_port->write_some(buf);
        // return true;,
    }
    bool isConnected(){return connected;}
};