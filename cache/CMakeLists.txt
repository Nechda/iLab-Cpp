cmake_minimum_required(VERSION 3.10)

project(iLab-LFU-cache)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_CXX_FLAGS "-Wall -Werror -g")

set(SRC main.cpp)

add_executable(cache ${SRC})
target_compile_definitions(cache PRIVATE LFU)

add_executable(perfect ${SRC})
target_compile_definitions(perfect PRIVATE PERFECT)

add_executable(incredible ${SRC})
target_compile_definitions(incredible PRIVATE PERFECT DONT_CACHE_SINGLES_PAGES)

add_executable(tester ${SRC})
target_compile_definitions(tester PRIVATE TESTS DONT_CACHE_SINGLES_PAGES)

find_program(CMAKE_PROGRAM cmake)
add_custom_target(
    end_to_end_testing
    COMMAND ${CMAKE_PROGRAM} -P "${CMAKE_SOURCE_DIR}/t/CMakeLists.txt"
)
add_dependencies(end_to_end_testing cache)
