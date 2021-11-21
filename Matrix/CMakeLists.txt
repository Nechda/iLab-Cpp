cmake_minimum_required(VERSION 3.10)

project(iLab-matrix)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_CXX_FLAGS "-Wall -Werror -g -O2")

find_package(Boost REQUIRED)

add_executable(matrix main.cpp)
target_link_libraries(matrix PRIVATE Boost::boost)

add_executable(tester main.cpp)
target_link_libraries(tester PRIVATE Boost::boost)
set_target_properties(tester PROPERTIES COMPILE_FLAGS "${CMAKE_CXX_FLAGS} -DTEST")
