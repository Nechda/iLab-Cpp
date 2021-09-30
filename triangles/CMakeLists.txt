cmake_minimum_required(VERSION 3.10)

project(iLab-triangles)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_CXX_FLAGS "-w -g")

add_executable(triangles main.cpp)