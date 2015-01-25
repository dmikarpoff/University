cmake_minimum_required(VERSION 2.8)

project(KNN)

include_directories(include)

add_executable(KNN              include/IClassfier.h
                                include/KNNClassifier.h
                                include/StreamHandler.h
                                src/KNNClassifier.cpp
                                src/StreamHandler.cpp
                                src/main.cpp)
