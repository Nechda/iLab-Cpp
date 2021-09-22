cmake_minimum_required(VERSION 3.10)

project(iLab-LFU-cache)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_CXX_FLAGS "-Wall -Werror")

add_executable(cache main.cpp perfect_cache.cpp LRU.cpp LFU.cpp)

add_executable(tester main.cpp perfect_cache.cpp LRU.cpp LFU.cpp)
set_target_properties(tester PROPERTIES COMPILE_FLAGS "${CMAKE_CXX_FLAGS} -DTESTS")
