cmake_minimum_required(VERSION 3.12)
project(LeafCapture VERSION 0.0.1 DESCRIPTION "Leaf capturing control code")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Boost)
include_directories( ${Boost_INCLUDE_DIR})


#find_package(OpenGL REQUIRED)
#find_package(GLUT REQUIRED)
#set(INCLUDE_DIRS ${OPENGL_INCLUDE_DIR} ${GLUT_INCLUDE_DIR})
#set(LIBRARY ${OPENGL_LIBRARIES} ${GLUT_LIBRARIES})

#include_directories(${INCLUDE_DIRS})

# OpenCV_DIR could also be read from system environment variable.
#if(WIN32)
#  set(OpenCV_DIR "d:/libs/opencv-2.4.8/build")
#else()
#  set(OpenCV_DIR "/usr/lib/opencv")
#endif()
#find_package(OpenCV REQUIRED COMPONENTS core imgproc highgui)
#include_directories(${OpenCV_INCLUDE_DIRS}) # not needed for opencv>=4.0
file(GLOB libusb "/usr/local/Cellar/libusb/1.0.26/lib/libusb-1.0.dylib")

set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

add_subdirectory("ext/glfw")

if(APPLE)
    list(APPEND EXTRA_LIBS "-framework OpenGL")
else()
    message("apple only")
endif()

add_executable(banboo 
Camera.h
Camera.cpp
Serial.h
Serial.cpp
Window.h
main.cpp)

#link_directories("/usr/local/lib")
file(GLOB DC1394 "/Users/dengxi/Desktop/libdc1394-2.2.3/dc1394/.libs/libdc1394.dylib")
message("DC1394 = ${DC1394}")

target_link_libraries(banboo LINK_PUBLIC 
${Boost_LIBRARIES} 
${OpenCV_INCLUDE_DIRS}
${DC1394} 
${libusb}
glfw
${EXTRA_LIBS})