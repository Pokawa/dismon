cmake_minimum_required(VERSION 3.13)
project(DisMon)
set(CMAKE_CXX_STANDARD 20)

add_subdirectory(include)

option(BUILD_TESTS "Build tests" ON)
if (BUILD_TESTS)
    enable_testing()
    add_subdirectory(test)
endif ()

add_subdirectory(example)