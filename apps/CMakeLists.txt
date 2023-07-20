add_executable(house house.cpp)

find_package(OpenGL REQUIRED)

target_link_libraries(house glfw OpenGL::GL glad glutils)
