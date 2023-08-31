cmake_minimum_required(VERSION 3.24)
project(Cerberus)

set(CMAKE_CXX_STANDARD 17)

add_executable(Cerberus src/main.cpp src/utils/logger.h src/utils/logger.cpp)
