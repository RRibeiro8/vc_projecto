cmake_minimum_required(VERSION 2.8)
project( projeto_vc )

find_package( OpenCV REQUIRED )

include_directories( ${OpenCV_INCLUDE_DIRS} ) 

set(SOURCES main.cpp spi.cpp spi.h)

add_executable( ${PROJECT_NAME} ${SOURCES} )
#add_executable( calib chessboard.cpp)

target_link_libraries( projeto_vc ${OpenCV_LIBS} leptonSDKEmb32PUB/Debug/libLEPTON_SDK.a )
#target_link_libraries( calib ${OpenCV_LIBS})

