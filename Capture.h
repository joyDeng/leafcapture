#pragma once


typedef enum CameraType{
    CCHAMELON=0,
    CCANON=1
} CameraType;

class CaptureDevice{



    public:
    CaptureDevice(){}
    CameraType capturetype(){return m_ctype;}
    CameraType m_ctype;
};