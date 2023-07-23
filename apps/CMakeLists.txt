find_package(OpenGL REQUIRED)

add_executable(house house.cpp)
target_link_libraries(house glfw OpenGL::GL glad glutils)

add_executable(glm_sandbox glm_sandbox.cpp)
target_link_libraries(glm_sandbox glm)