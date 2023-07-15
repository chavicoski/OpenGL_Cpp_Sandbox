cmake_minimum_required(VERSION 3.0.0)
project(hello_window VERSION 0.1.0)

add_executable(hello_window src/main.cpp)

set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
add_subdirectory(dependencies/glfw)
target_include_directories(hello_window PRIVATE dependencies/glfw/include)
target_link_libraries(hello_window glfw)