find_package(OpenGL REQUIRED)

add_executable(house house.cpp)
target_link_libraries(house glfw OpenGL::GL glad glm glutils)

add_executable(lighting lighting.cpp)
target_link_libraries(lighting glfw OpenGL::GL glad glm glutils)

add_executable(glm_sandbox glm_sandbox.cpp)
target_link_libraries(glm_sandbox glm)