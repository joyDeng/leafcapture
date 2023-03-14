cmake_minimum_required(VERSION 3.12)
project(LeafCapture)

#set(CMAKE_C_COMPILER clang)
#set(CMAKE_CXX_COMPILER clang)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Boost COMPONENTS system filesystem REQUIRED)
include_directories( ${Boost_INCLUDE_DIR})

add_definitions(-DGL_SILENCE_DEPRECATION)


find_package(OpenGL REQUIRED)
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
find_package(OpenCV REQUIRED COMPONENTS core imgproc highgui imgcodecs photo)
include_directories(${OpenCV_INCLUDE_DIRS}) # not needed for opencv>=4.0
# file(GLOB libusb "/usr/local/Cellar/libusb/1.0.26/lib/libusb-1.0.dylib")

set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

add_subdirectory("ext/glfw")

# add_library(STB_IMAGE "ext/stb/stb_image.h")

# if(APPLE)
#     list(APPEND EXTRA_LIBS "-framework OpenGL")
# else()
#     message("apple only")
# endif()

add_executable(banboo 
Camera.h
Capture.h
Camera2.h
Camera.cpp
Serial.h
Serial.cpp
Window.h
main.cpp)

add_executable(viewer 
Camera.h
Capture.h
Camera2.h
Camera.cpp
Serial.h
Serial.cpp
Window.h
viewer.cpp)

add_executable(calibrate 
Camera.h
Capture.h
Camera2.h
Camera.cpp
Serial.h
Serial.cpp
Window.h
calibrate.cpp)

add_executable(turnoff 
Camera.h
Capture.h
Camera2.h
Camera.cpp
Serial.h
Serial.cpp
Window.h
turnoff.cpp)

#link_directories("/usr/local/lib")
set(Vimba_DIR "/opt/Vimba_6_0/")
set(STB_INCLUDE "ext/stb/")
# message("DC1394 = ${DC1394}")

get_filename_component( VIMBA_INC_DIR "${Vimba_DIR}" ABSOLUTE)
get_filename_component( VIMBA_TRANSFORM_INC_DIR "${Vimba_DIR}/VimbaImageTransform/Include" ABSOLUTE)
get_filename_component( VIMBA_MES_INC_DIR "${Vimba_DIR}/VimbaCPP/Examples" ABSOLUTE)


FIND_LIBRARY(LIBUSB_INLCUDE_DIR NAMES libusb.h PATH_SUFFIXES "include" "libusb" "libusb-1.0")
FIND_LIBRARY(LIBUSB_LIBRARY NAMES usb PATH_SUFFIXES "lib" "lib32" "lib64")
FIND_LIBRARY(LIBDC1394_LIBRARY NAMES libdc1394.so PATHS "/usr/local/lib/")
FIND_LIBRARY(VIMBACPP_LIB NAMES libVimbaCPP.so PATHS "/opt/Vimba_6_0/VimbaCPP/DynamicLib/x86_64bit/")
FIND_LIBRARY(VIMBA_TRANS_LIB NAMES libVimbaC.so PATHS "/opt/Vimba_6_0/VimbaCPP/DynamicLib/x86_64bit/")
FIND_LIBRARY(VIMBAC_LIB NAMES libVimbaImageTransform.so PATHS "/opt/Vimba_6_0/VimbaImageTransform/DynamicLib/x86_64bit/")

list( APPEND VIMBA_LIBS ${VIMBACPP_LIB} )
list( APPEND VIMBA_LIBS ${VIMBA_TRANS_LIB} )
list( APPEND VIMBA_LIBS ${VIMBAC_LIB} )
list( APPEND DC_LIBS ${LIBDC1394_LIBRARY})

message("VIMBA_LIBS = ${VIMBA_LIBS}")

target_include_directories(banboo 
PUBLIC ${VIMBA_INC_DIR}
PUBLIC ${VIMBA_TRANSFORM_INC_DIR}
PUBLIC ${VIMBA_MES_INC_DIR}
PUBLIC ${STB_INCLUDE} 
PUBLIC ${OpenCV_INCLUDE_DIRS})

target_include_directories(turnoff 
PUBLIC ${VIMBA_INC_DIR}
PUBLIC ${VIMBA_TRANSFORM_INC_DIR}
PUBLIC ${VIMBA_MES_INC_DIR}
PUBLIC ${STB_INCLUDE} 
PUBLIC ${OpenCV_INCLUDE_DIRS})

target_include_directories(viewer 
PUBLIC ${VIMBA_INC_DIR}
PUBLIC ${VIMBA_TRANSFORM_INC_DIR}
PUBLIC ${VIMBA_MES_INC_DIR}
PUBLIC ${STB_INCLUDE} 
PUBLIC ${OpenCV_INCLUDE_DIRS})

target_include_directories(calibrate 
PUBLIC ${VIMBA_INC_DIR}
PUBLIC ${VIMBA_TRANSFORM_INC_DIR}
PUBLIC ${VIMBA_MES_INC_DIR}
PUBLIC ${STB_INCLUDE} 
PUBLIC ${OpenCV_INCLUDE_DIRS})


# -lboost_filesystem -lboost_system

target_link_libraries(banboo LINK_PUBLIC 
${Boost_LIBRARIES} 
${Boost_FILESYSTEM_LIBRARY}
${Boost_SYSTEM_LIBRARY}
# ${DC1394} 
# ${libusb}
usb-1.0
# dc1394
glfw
# STB_IMAGE
OpenGL
${OpenCV_LIBS}
# ${LIBVIMBA}
${DC_LIBS}
${VIMBA_LIBS})

target_link_libraries(turnoff LINK_PUBLIC 
${Boost_LIBRARIES} 
${Boost_FILESYSTEM_LIBRARY}
${Boost_SYSTEM_LIBRARY}
# ${DC1394} 
# ${libusb}
usb-1.0
# dc1394
glfw
# STB_IMAGE
OpenGL
${OpenCV_LIBS}
# ${LIBVIMBA}
${DC_LIBS}
${VIMBA_LIBS})

target_link_libraries(viewer LINK_PUBLIC 
${Boost_LIBRARIES} 
${Boost_FILESYSTEM_LIBRARY}
${Boost_SYSTEM_LIBRARY}
usb-1.0
glfw
OpenGL
${OpenCV_LIBS}
${DC_LIBS}
${VIMBA_LIBS})

target_link_libraries(calibrate LINK_PUBLIC 
${Boost_LIBRARIES} 
${Boost_FILESYSTEM_LIBRARY}
${Boost_SYSTEM_LIBRARY}
usb-1.0
glfw
OpenGL
${OpenCV_LIBS}
${DC_LIBS}
${VIMBA_LIBS})

target_link_libraries(turnoff LINK_PUBLIC 
${Boost_LIBRARIES} 
${Boost_FILESYSTEM_LIBRARY}
${Boost_SYSTEM_LIBRARY}
usb-1.0
${DC_LIBS}
)