cmake_minimum_required(VERSION 2.8)

project("Logit regression")

find_package(OpenCV REQUIRED)

include_directories(include ${OpenCV_INCLUDE_DIRS})

add_executable(solution                 include/IClassifier.h
                                        include/LogitSolver.h
                                        include/StreamHandler.h
                                        src/LogitSolver.cpp
                                        src/main.cpp
                                        src/StreamHandler.cpp)

target_link_libraries(solution      ${OpenCV_LIBS})
