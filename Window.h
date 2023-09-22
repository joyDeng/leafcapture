// #define GLFW_INCLUDE_NONE
// #include <glad/gl.h>
#pragma once
#include <GLFW/glfw3.h>
#include "AllivedCameras.h"

#include <chrono>
#include <thread>
using namespace std::chrono;




class ImageWindow{
    public:
    GLFWwindow* window;
    CameraC *m_camera;
    GLuint tex_handle;
    unsigned int width = 640;
    unsigned int height = 480;

    
 
    ImageWindow(){
    /* Initialize the library */
        if (!glfwInit())
          exit(0);
       
        m_camera = new CameraC();
        if(m_camera->created){
            // m_camera->setupCapture(50);
            auto start = high_resolution_clock::now();
            m_camera->shot();
            m_camera->getImageSize();
            
            auto stop = high_resolution_clock::now();
            auto duration = duration_cast<microseconds>(stop - start);
            std::cout<<"takeing shot will spend us"<<duration.count()<<" us"<<std::endl;
            width = m_camera->m_width;
            height = m_camera->m_height;
            std::cout<<"length: "<<width<<"x"<<height<<std::endl;
        }

     /* Create a windowed mode window and its OpenGL context */
        window = glfwCreateWindow(width, height, "Hello World", NULL, NULL);

        glfwMakeContextCurrent(window);
        glGenTextures(1, &tex_handle);
        glBindTexture(GL_TEXTURE_2D, tex_handle);

        std::cout<<"texture created"<<std::endl;
    }


    bool launchWindow(){
        if (!window)
        {
            glfwTerminate();
            return false;
        }

        std::cout<<"luanching window"<<std::endl;
        /* Make the window's context current */
        glfwMakeContextCurrent(window);

        int texture_width = width;
        int texture_height = height;

        // unsigned char data[texture_width * texture_height * 3];
        // for(int i=0 ; i < texture_width ; i++){
        //     for(int j=0 ; j < texture_height ; j++){
        //         data[texture_width * j * 3 + i * 3 + 0] = 0xff;
        //         data[texture_width * j * 3 + i * 3 + 1] = 0x00;
        //         data[texture_width * j * 3 + i * 3 + 2] = 0x00;
        //     }
        // }

        // unsigned char *dataptr;
        // dataptr = (unsigned char *) m_camera->getImage(0);


        float *dataptr;
        dataptr = new float[m_camera->m_width * m_camera->m_height];
        m_camera->getbuffer(dataptr);

        // glfwMakeContextCurrent(window);
        // std::cout<<"writing data"<<std::endl;
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
        glTexImage2D(GL_TEXTURE_2D, 0,  GL_RED, texture_width, texture_height, 0,  GL_RED,  GL_FLOAT, dataptr);


        /* Loop until the user closes the window */
        while (!glfwWindowShouldClose(window))
        {
            /* Render here */
            glClear(GL_COLOR_BUFFER_BIT);

            if(m_camera->created){
                m_camera->shot();
                // dataptr = (unsigned char *) m_camera->getImage(0);
                m_camera->getbuffer(dataptr);
                glTexImage2D(GL_TEXTURE_2D, 0,  GL_RED, texture_width, texture_height, 0,  GL_RED,  GL_FLOAT, dataptr);
            }
            /* set up ortho projection */
            int frame_width, frame_height;
            glfwGetFramebufferSize(window, &frame_width, &frame_height);
            glMatrixMode(GL_PROJECTION);
            glLoadIdentity();
            glOrtho(0, frame_width, 0, frame_height, -1, 1);
            glMatrixMode(GL_MODELVIEW);

            glEnable(GL_TEXTURE_2D);
            glBindTexture(GL_TEXTURE_2D, tex_handle);
            glBegin(GL_QUADS);
                glTexCoord2d(0, 0); glVertex2i(0, frame_height);
                glTexCoord2d(1, 0); glVertex2i(frame_width, frame_height);
                glTexCoord2d(1, 1); glVertex2i(frame_width, 0);
                glTexCoord2d(0, 1); glVertex2i(0, 0);
            glEnd();
            glDisable(GL_TEXTURE_2D);

            // if(m_camera==nullptr)
            //     glDrawPixels(100, 100, GL_RGB, GL_UNSIGNED_BYTE, data);
            // else{
            //     m_camera->shot();
            //     glDrawPixels(m_camera->width, m_camera->height, GL_RGB, GL_UNSIGNED_BYTE, m_camera->frame->image);
            // }
                
            /* Swap front and back buffers */
            glfwSwapBuffers(window);

            /* Poll for and process events */
            glfwPollEvents();
        }

        // start = 0;

        glfwTerminate();
        delete dataptr;
        return true;
    }

    ~ImageWindow(){
        printf("free m camera");
        delete m_camera;
        printf("free window");
        free(window);
    }
};