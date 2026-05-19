cmake_minimum_required(VERSION 3.19)

project(TextEditor)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

set(CMAKE_DISABLE_IN_SOURCE_BUILDS ON)

add_executable(TextEditor src/main.cpp)
