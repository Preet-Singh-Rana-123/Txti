cmake_minimum_required(VERSION 3.19)

project(TextEditor)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

set(CMAKE_DISABLE_IN_SOURCE_BUILDS ON)

find_package(Curses REQUIRED)

add_executable(TextEditor 
    src/main.cpp
    src/textEditor.cpp
)

target_link_libraries(TextEditor PRIVATE ${CURSES_LIBRARIES})

target_include_directories(TextEditor PRIVATE ${CURSES_INCLUDE_DIRS})
