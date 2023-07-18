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

void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
  glViewport(0, 0, width, height);
}

float triangle_vertices[] = {
    -0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, // Bottom-left, Red
    0.5f,  -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, // Bottom-right, Green
    0.0f,  0.5f,  0.0f, 0.0f, 0.0f, 1.0f, // Upper-center, Blue
};

unsigned int draw_triangle() {
  // Prepare the buffer to sed the vertex data
  unsigned int VBO;
  glGenBuffers(1, &VBO);
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  // Push the vertex data into the buffer
  glBufferData(GL_ARRAY_BUFFER, sizeof(triangle_vertices), triangle_vertices,
               GL_STATIC_DRAW);
  // Prepare the vertex position attribute
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)0);
  glEnableVertexAttribArray(0);
  // Prepare the vertex color attribute
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float),
                        (void *)(3 * sizeof(float)));
  glEnableVertexAttribArray(1);

  return VBO;
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

  // Ensure that the OpenGL viewport is adjusted to the window size
  int frameWidth, frameHeight;
  glfwGetFramebufferSize(window, &frameWidth, &frameHeight);
  glViewport(0, 0, frameWidth, frameHeight);
  // Set a callback to adjust the viewport when resizing the window
  glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

  glClearColor(0.25f, 0.5f, 0.75f, 1.0f);

  unsigned int shader =
      make_shader("../src/shaders/vertex.txt", "../src/shaders/fragment.txt");

  unsigned int triangle_VAO;
  glGenVertexArrays(1, &triangle_VAO);
  glBindVertexArray(triangle_VAO);
  unsigned int triangle_VBO = draw_triangle();

  while (!glfwWindowShouldClose(window)) {
    // Read used input
    glfwPollEvents();
    // Clear the screen
    glClear(GL_COLOR_BUFFER_BIT);
    // Render
    glUseProgram(shader);
    glBindVertexArray(triangle_VAO);
    glDrawArrays(GL_TRIANGLES, 0, 3);
    // Display the updated rendered data
    glfwSwapBuffers(window);
  }

  glDeleteProgram(shader);
  glfwTerminate();
  return 0;
}
