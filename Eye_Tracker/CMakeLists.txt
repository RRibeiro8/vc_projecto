cmake_minimum_required(VERSION 2.8)
project( projeto_vc )

find_package( OpenCV REQUIRED )

add_definitions(-std=c++11)

include_directories( ${OpenCV_INCLUDE_DIRS} ) 

set(SOURCES Main.cpp PupilDetector.cpp struct/CircularQueue.cpp object/Eye.cpp object/Face.cpp object/Nose.cpp 
math/Circle.cpp math/RectUtils.cpp math/UUIDGenerator.cpp) 

add_executable( eye_tracker ${SOURCES} )

target_link_libraries( eye_tracker ${OpenCV_LIBS} )

