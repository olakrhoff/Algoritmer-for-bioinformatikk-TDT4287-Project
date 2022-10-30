cmake_minimum_required(VERSION 3.20)
project(task5)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_C_FLAGS -O3)

add_executable(task5 main.cpp ../task4/keyword_tree.cpp)
