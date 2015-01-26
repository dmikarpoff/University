cmake_minimum_required(VERSION 2.8)

project("Associative rules")

include_directories(include)

add_executable(solver           src/main.cpp
                                include/Normalizer.h
                                src/Normalizer.cpp
                                include/Solver.h
                                src/Solver.cpp)
