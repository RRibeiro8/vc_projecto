cmake_minimum_required(VERSION 2.8)
project( projeto_vc )

find_package( OpenCV REQUIRED )

add_definitions(-std=c++11)

include_directories( ${OpenCV_INCLUDE_DIRS} ) 

set(SOURCES Main.cpp PupilDetector.cpp struct/CircularQueue.cpp object/Eye.cpp object/Face.cpp object/Nose.cpp 
math/Circle.cpp math/RectUtils.cpp math/UUIDGenerator.cpp) 

add_executable( ${PROJECT_NAME} ${SOURCES} )

target_link_libraries( projeto_vc ${OpenCV_LIBS} /home/pi/Desktop/Ribeiro/Thermal_camera/leptonSDKEmb32PUB/Debug/libLEPTON_SDK.a )

