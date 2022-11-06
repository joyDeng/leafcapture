// #define GLFW_INCLUDE_NONE
// #include <glad/gl.h>
#include <GLFW/glfw3.h>


class ImageWindow{
    GLFWwindow* window;
    
    public:
    ImageWindow(){
    /* Initialize the library */
        if (!glfwInit())
          exit(0);

     /* Create a windowed mode window and its OpenGL context */
        window = glfwCreateWindow(640, 480, "Hello World", NULL, NULL);
    }

    bool launchWindow(){
            if (!window)
        {
            glfwTerminate();
            return false;
        }

        /* Make the window's context current */
        glfwMakeContextCurrent(window);
        unsigned char data[100 * 100 * 3];
        for(int i=0 ; i < 100 ; i++){
            for(int j=0 ; j < 100 ; j++){
                data[100 * j * 3 + i * 3 + 0] = 0xff;
                data[100 * j * 3 + i * 3 + 1] = 0x00;
                data[100 * j * 3 + i * 3 + 2] = 0x00;
            }
        }

        /* Loop until the user closes the window */
        while (!glfwWindowShouldClose(window))
        {
            /* Render here */
            glClear(GL_COLOR_BUFFER_BIT);
            glDrawPixels(100, 100, GL_RGB, GL_UNSIGNED_BYTE, data);

            /* Swap front and back buffers */
            glfwSwapBuffers(window);

            /* Poll for and process events */
            glfwPollEvents();
        }

        glfwTerminate();
        return true;
    }
};