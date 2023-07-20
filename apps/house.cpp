#include <glutils.hpp>
#include <iostream>

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
  // Prepare the Element Buffer Object for `index drawing`
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

  unsigned int shader = make_shader("../../src/shaders/vertex.txt",
                                    "../../src/shaders/fragment.txt");

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
