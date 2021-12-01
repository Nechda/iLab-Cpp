cmake_minimum_required(VERSION 3.10)

project(iLab-tree)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_CXX_FLAGS "-Wall -Werror -g -O2")

set(SRC main.cpp)

add_executable(tree ${SRC})

add_executable(tester ${SRC})
target_compile_definitions(tester PRIVATE TEST)
