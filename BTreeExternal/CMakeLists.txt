cmake_minimum_required(VERSION 2.8)

project(B-Tree)

include_directories(include)

add_definitions(-std=c++0x)

add_executable(b_tree_test          src/main.cpp
                                    include/IExternalReader.h
                                    include/BTree.hpp
                                    include/ExternalReaderDammy.h
                                    src/ExternalReaderDammy.cpp
                                    include/Interpreter.hpp)
