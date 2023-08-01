cmake_minimum_required(VERSION 3.14)
project(hello_window VERSION 0.1.0 DESCRIPTION "Executable project to experiment with OpenGL")
set(CMAKE_CXX_STANDARD 20)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# External dependencies
include_directories(dependencies)
add_subdirectory(dependencies)

# Library code
add_subdirectory(src)

# Executable code
add_subdirectory(apps)
