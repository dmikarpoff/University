cmake_minimum_required(VERSION 2.8)

project("Random forest")

include_directories(include)

add_executable(solver           src/main.cpp
                                src/DecisionTree.cpp
                                include/DecisionTree.h)
