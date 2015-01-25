cmake_minimum_required(VERSION 2.8)

project("Linear regression")

include_directories(include)

add_executable(solver               include/IClassifier.h
                                    include/RegressionSolver.h
                                    include/StreamHandler.h
                                    src/main.cpp
                                    src/RegressionSolver.cpp
                                    src/StreamHandler.cpp)
