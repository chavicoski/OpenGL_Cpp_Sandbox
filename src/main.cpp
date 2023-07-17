#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <fstream>
#include <glad/glad.h>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

unsigned int make_module(const std::string &filepath,
                         const unsigned int module_type) {
  std::ifstream file;
  std::stringstream bufferedLines;
  std::string line;
  // Read the module source code from file
  file.open(filepath);
  while (std::getline(file, line)) {
    bufferedLines << line << "\n";
  }
  file.close();
  // Convert the code to a C-like string for OpenGL
  std::string auxShaderSource = bufferedLines.str();
  const char *ShaderSource = auxShaderSource.c_str();

  // Create and compile the shader module
  unsigned int shaderModule = glCreateShader(module_type);
  glShaderSource(shaderModule, 1, &ShaderSource, NULL);
  glCompileShader(shaderModule);

  // Check if the compilation was successful
  int success;
  glGetShaderiv(shaderModule, GL_COMPILE_STATUS, &success);
  if (!success) {
    char errorLog[1024];
    glGetShaderInfoLog(shaderModule, 1024, NULL, errorLog);
    std::cout << "Error in shader compilation:\n" << errorLog << std::endl;
  }

  return shaderModule;
}

unsigned int make_shader(const std::string &vertex_filepath,
                         const std::string &fragment_filepath) {
  // Create the compiled shaders
  std::vector<unsigned int> modules;
  modules.push_back(make_module(vertex_filepath, GL_VERTEX_SHADER));
  modules.push_back(make_module(fragment_filepath, GL_FRAGMENT_SHADER));

  // Attach the shader modules to the shader program
  unsigned int shader = glCreateProgram();
  for (unsigned int shaderModule : modules) {
    glAttachShader(shader, shaderModule);
  }
  // Link the modules
  glLinkProgram(shader);

  // Check linking status
  int success;
  glGetProgramiv(shader, GL_LINK_STATUS, &success);
  if (!success) {
    char errorLog[1024];
    glGetProgramInfoLog(shader, 1024, NULL, errorLog);
    std::cout << "Error in shader linking:\n" << errorLog << std::endl;
  }

  // After linking the shaders source can be deleted
  for (unsigned int shaderModule : modules) {
    glDeleteShader(shaderModule);
  }

  return shader;
}

int main() {
  // Initialize the window manager
  if (!glfwInit()) {
    std::cout << "GLFW could not be initialized" << std::endl;
    exit(EXIT_FAILURE);
  }
  // Set the OpenGL version to 3.3 with core-profile
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

  // Initialize the window
  GLFWwindow *window = glfwCreateWindow(640, 480, "OpenGL Sandbox", NULL, NULL);
  if (!window) {
    glfwTerminate();
    exit(EXIT_FAILURE);
  }
  // Set the window as the OpenGL context
  glfwMakeContextCurrent(window);

  // Load OpenGL
  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    glfwTerminate();
    return 1;
  }

  glClearColor(0.25f, 0.5f, 0.75f, 1.0f);

  unsigned int shader =
      make_shader("../src/shaders/vertex.txt", "../src/shaders/fragment.txt");

  while (!glfwWindowShouldClose(window)) {
    glfwPollEvents();
    glClear(GL_COLOR_BUFFER_BIT);
    glUseProgram(shader);
    glfwSwapBuffers(window);
  }

  glDeleteProgram(shader);
  glfwTerminate();
  return 0;
}
