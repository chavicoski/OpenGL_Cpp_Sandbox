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

void draw_triangle() {
  // Triangle vertices data (position and color)
  constexpr float triangle_vertices[18] = {
      -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, // Bottom-left, Red
      0.5f,  0.0f, 0.0f, 0.0f, 1.0f, 0.0f, // Bottom-right, Green
      0.0f,  0.5f, 0.0f, 0.0f, 0.0f, 1.0f, // Top-center, Blue
  };
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
}

void draw_rectangle() {
  // Rectangle unique vertices data (postion and color)
  constexpr float rectangle_vertices[24] = {
      0.5f,  0.0f,  0.0f, 0.0f, 1.0f, 0.0f, // Top-right
      0.5f,  -0.5f, 0.0f, 0.0f, 1.0f, 1.0f, // Bottom-right
      -0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 1.0f, // Bottom-left
      -0.5f, 0.0f,  0.0f, 1.0f, 0.0f, 0.0f  // Top-left
  };
  // Order to draw the unique vertices to form the rectangle
  constexpr unsigned int indices[6] = {
      0, 1, 3, // Top triangle
      1, 2, 3  // Bottom triangle
  };
  // Prepare the Elements Buffer Object for `index drawing`
  unsigned int EBO;
  glGenBuffers(1, &EBO);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
  // Push the vertex indexes into the buffer
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices,
               GL_STATIC_DRAW);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
  glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
  // Prepare the vertex buffer for the unique vertices data
  unsigned int VBO;
  glGenBuffers(1, &VBO);
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  // Push the vertex data into the buffer
  glBufferData(GL_ARRAY_BUFFER, sizeof(rectangle_vertices), rectangle_vertices,
               GL_STATIC_DRAW);
  // Prepare the vertex position attribute
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)0);
  glEnableVertexAttribArray(0);
  // Prepare the vertex color attribute
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float),
                        (void *)(3 * sizeof(float)));
  glEnableVertexAttribArray(1);
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

  // Store the triangle draw config in a Vertex Array Object
  unsigned int triangle_VAO;
  glGenVertexArrays(1, &triangle_VAO);
  glBindVertexArray(triangle_VAO);
  draw_triangle();

  // Store the rectangle draw config in a Vertex Array Object
  unsigned int rectangle_VAO;
  glGenVertexArrays(1, &rectangle_VAO);
  glBindVertexArray(rectangle_VAO);
  draw_rectangle();

  while (!glfwWindowShouldClose(window)) {
    // Read used input
    glfwPollEvents();

    // Clear the screen
    glClear(GL_COLOR_BUFFER_BIT);

    // Prepare the shaders to draw
    glUseProgram(shader);

    // Draw the triangle
    glBindVertexArray(triangle_VAO);
    glDrawArrays(GL_TRIANGLES, 0, 3);

    // Draw the rectangle
    glBindVertexArray(rectangle_VAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

    // Display the updated rendered data
    glfwSwapBuffers(window);
  }

  glDeleteProgram(shader);
  glfwTerminate();
  return 0;
}
