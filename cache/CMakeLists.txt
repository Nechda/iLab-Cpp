cmake_minimum_required(VERSION 3.10)

project(iLab-2Q-cache)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_CXX_FLAGS "-Wall -Werror")

add_executable(cache main.cpp perfect_cache.cpp LRU.cpp)