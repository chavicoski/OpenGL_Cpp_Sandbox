#include <GLFW/glfw3.h>
#include <iostream>

int main() {
  GLFWwindow *window;

  if (!glfwInit()) {
    std::cout << "GLFW could not be initialized" << std::endl;
    return 1;
  }

  window = glfwCreateWindow(640, 480, "OpenGL Sandbox", NULL, NULL);

  while (!glfwWindowShouldClose(window)) {
    glfwPollEvents();
  }

  glfwTerminate();
  return 0;
}