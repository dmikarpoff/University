cmake_minimum_required(VERSION 2.8)

project(Perceptron)

find_package( OpenCV REQUIRED )
include_directories( ${OpenCV_INCLUDE_DIRS} )

add_executable(solver           src/main.cpp)

target_link_libraries(solver ${OpenCV_LIBS})
