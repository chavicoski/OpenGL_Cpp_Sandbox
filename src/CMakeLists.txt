add_library(glutils glutils.cpp ../include/glutils.hpp)

target_include_directories(glutils PUBLIC ../include)

target_link_libraries(glutils glfw)
