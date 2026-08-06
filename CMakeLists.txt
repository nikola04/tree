cmake_minimum_required(VERSION 3.20)

project(tree LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(tree src/main.cpp
    src/tree/ObjectPrinter.cpp
    src/cli/Parser.cpp)

install(TARGETS tree DESTINATION bin)