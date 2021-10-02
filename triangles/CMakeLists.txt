cmake_minimum_required(VERSION 3.10)

project(iLab-triangles)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_CXX_FLAGS "-Wall -Werror -g -O2")

add_executable(triangles main.cpp triangle.cpp)

add_executable(tester main.cpp triangle.cpp)
set_target_properties(tester PROPERTIES COMPILE_FLAGS "${CMAKE_CXX_FLAGS} -DINTERSECTED_FUNCTION_TEST")

find_program(BASH_PROGRAM bash)
enable_testing()
if(BASH_PROGRAM)
    add_test(
        NAME triangle_intersection_1
        COMMAND ${BASH_PROGRAM} -c "./tester < ${CMAKE_CURRENT_SOURCE_DIR}/t/1.t"
    )
    add_test(
        NAME triangle_intersection_2
        COMMAND ${BASH_PROGRAM} -c "./tester < ${CMAKE_CURRENT_SOURCE_DIR}/t/2.t"
    )
    add_test(
        NAME triangle_intersection_3
        COMMAND ${BASH_PROGRAM} -c "./tester < ${CMAKE_CURRENT_SOURCE_DIR}/t/3.t"
    )
    add_test(
        NAME triangle_intersection_4
        COMMAND ${BASH_PROGRAM} -c "./tester < ${CMAKE_CURRENT_SOURCE_DIR}/t/4.t"
    )
    add_test(
        NAME triangle_intersection_5
        COMMAND ${BASH_PROGRAM} -c "./tester < ${CMAKE_CURRENT_SOURCE_DIR}/t/5.t"
    )
endif(BASH_PROGRAM)