cmake_minimum_required(VERSION 3.20)
project(task4)

set(CMAKE_CXX_STANDARD 23)

set(CMAKE_OSX_ARCHITECTURES "arm64")

add_executable(task4 main.cpp keyword_tree.cpp)
