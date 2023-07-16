#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include <iostream>

int main() {
  GLFWwindow *window;
  // Initialize the window manager
  if (!glfwInit()) {
    std::cout << "GLFW could not be initialized" << std::endl;
    exit(EXIT_FAILURE);
  }
  // Initialize the window
  window = glfwCreateWindow(640, 480, "OpenGL Sandbox", NULL, NULL);
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

  while (!glfwWindowShouldClose(window)) {
    glfwPollEvents();
    glClear(GL_COLOR_BUFFER_BIT);
    glfwSwapBuffers(window);
  }

  glfwTerminate();
  return 0;
}