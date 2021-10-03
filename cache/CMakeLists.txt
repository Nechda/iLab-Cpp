cmake_minimum_required(VERSION 3.10)

project(iLab-LFU-cache)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_CXX_FLAGS "-Wall -Werror -g")

set(SRC main.cpp)
macro(add_define tar def)
    set_target_properties(${tar} PROPERTIES COMPILE_FLAGS "${CMAKE_CXX_FLAGS} -D${def}")
endmacro()

add_executable(cache ${SRC})
add_define(cache "LFU")

add_executable(perfect ${SRC})
add_define(perfect "PERFECT")

add_executable(tester ${SRC})
add_define(tester "TESTS")

find_program(CMAKE_PROGRAM cmake)
add_custom_target(
    end_to_end_testing
    COMMAND ${CMAKE_PROGRAM} -P "${CMAKE_SOURCE_DIR}/t/CMakeLists.txt"
)
add_dependencies(end_to_end_testing cache)
