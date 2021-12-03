cmake_minimum_required(VERSION 3.10)

project(iLab-tree)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_CXX_FLAGS "-Wall -Werror -g")

set(SRC main.cpp)

add_executable(tree ${SRC})
set(RELEASE_ADDITION_OPTIONS -O2)
target_compile_options(tree PRIVATE ${RELEASE_ADDITION_OPTIONS})
target_link_options(tree PRIVATE ${RELEASE_ADDITION_OPTIONS})
target_compile_definitions(tree PRIVATE NDEBUG)


add_executable(tester ${SRC})
set(TESTER_ADDITION_OPTIONS -O0 -fsanitize=leak,address,undefined)
target_compile_options(tester PRIVATE ${TESTER_ADDITION_OPTIONS})
target_link_options(tester PRIVATE ${TESTER_ADDITION_OPTIONS})
target_compile_definitions(tester PRIVATE TEST)
