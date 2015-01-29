cmake_minimum_required(VERSION 2.8)

project(Bayes)

add_definitions(-std=c++0x)

find_package(Boost REQUIRED COMPONENTS system filesystem)

include_directories(            include
                                ${Boost_INCLUDE_DIRS})

add_executable(solver           src/main.cpp
                                include/DataHolder.h
                                src/DataHolder.cpp)

target_link_libraries(solver    ${Boost_LIBRARIES})
