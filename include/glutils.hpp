#pragma once

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include <string>

GLuint make_module(const std::string &filepath, const GLuint module_type);

GLuint make_shader(const std::string &vertex_filepath,
                   const std::string &fragment_filepath);

void framebuffer_size_callback(GLFWwindow *window, int width, int height);