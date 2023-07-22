#include <glutils.hpp>
#include <iostream>
#include <string>
#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"

GLuint texture_setup(const std::string &filepath) {
  // Generate the OpenGL texture object
  GLuint texture;
  glGenTextures(1, &texture);
  // Bind the texture to the texture unit 0
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, texture);
  // Set the texture wrapping/filtering options
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                  GL_LINEAR_MIPMAP_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  // Load the texture data from file
  int width, height, nrChannels;
  unsigned char *data =
      stbi_load(filepath.c_str(), &width, &height, &nrChannels, 0);
  if (data) {
    // Generate the 2D texture by setting the data and format
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB,
                 GL_UNSIGNED_BYTE, data);
    // Generate all the Mipmap levels up to 1-pixel-size
    glGenerateMipmap(GL_TEXTURE_2D);
  } else {
    std::cout << "Error during texture data loading" << std::endl;
  }
  // Now we can free the source texture data
  stbi_image_free(data);
  return texture;
}

void draw_triangle() {
  // Triangle vertices data
  // Format: postion(x, y, z), color(r, g, b), texCoord(x, y)
  constexpr float triangle_vertices[24] = {
      -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, // Bottom-left, Red
      0.5f,  0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, // Bottom-right, Green
      0.0f,  0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 0.5f, 1.0f  // Top-center, Blue
  };
  // Prepare the buffer to sed the vertex data
  GLuint VBO;
  glGenBuffers(1, &VBO);
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  // Push the vertex data into the buffer
  glBufferData(GL_ARRAY_BUFFER, sizeof(triangle_vertices), triangle_vertices,
               GL_STATIC_DRAW);
  // Prepare the vertex position attribute
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)0);
  glEnableVertexAttribArray(0);
  // Prepare the vertex color attribute
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float),
                        (void *)(3 * sizeof(float)));
  glEnableVertexAttribArray(1);
  // Prepare the texture coordinates attribute
  glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float),
                        (void *)(6 * sizeof(float)));
  glEnableVertexAttribArray(2);
}

void draw_rectangle() {
  // Rectangle unique vertices data
  // Format: postion(x, y, z), color(r, g, b), texCoord(x, y)
  constexpr float rectangle_vertices[32] = {
      0.5f,  0.0f,  0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f, // Top-right
      0.5f,  -0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, 0.0f, // Bottom-right
      -0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, // Bottom-left
      -0.5f, 0.0f,  0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f  // Top-left
  };
  // Order to draw the unique vertices to form the rectangle
  constexpr GLuint indices[6] = {
      0, 1, 3, // Top triangle
      1, 2, 3  // Bottom triangle
  };
  // Prepare the Element Buffer Object for `index drawing`
  GLuint EBO;
  glGenBuffers(1, &EBO);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
  // Push the vertex indexes into the buffer
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices,
               GL_STATIC_DRAW);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
  glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
  // Prepare the vertex buffer for the unique vertices data
  GLuint VBO;
  glGenBuffers(1, &VBO);
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  // Push the vertex data into the buffer
  glBufferData(GL_ARRAY_BUFFER, sizeof(rectangle_vertices), rectangle_vertices,
               GL_STATIC_DRAW);
  // Prepare the vertex position attribute
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)0);
  glEnableVertexAttribArray(0);
  // Prepare the vertex color attribute
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float),
                        (void *)(3 * sizeof(float)));
  glEnableVertexAttribArray(1);
  // Prepare the texture coordinates attribute
  glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float),
                        (void *)(6 * sizeof(float)));
  glEnableVertexAttribArray(2);
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

  GLuint shader = make_shader("../../src/shaders/vertex.txt",
                              "../../src/shaders/fragment.txt");

  // Prepare the triangle texture
  GLuint triangle_tex = texture_setup("../../textures/roof.png");
  // Store the triangle draw config in a Vertex Array Object
  GLuint triangle_VAO;
  glGenVertexArrays(1, &triangle_VAO);
  glBindVertexArray(triangle_VAO);
  draw_triangle();

  // Prepare the rectangle texture
  GLuint rectangle_tex = texture_setup("../../textures/container.jpg");
  // Store the rectangle draw config in a Vertex Array Object
  GLuint rectangle_VAO;
  glGenVertexArrays(1, &rectangle_VAO);
  glBindVertexArray(rectangle_VAO);
  draw_rectangle();

  GLint texLocation = glGetUniformLocation(shader, "ourTexture");

  while (!glfwWindowShouldClose(window)) {
    // Read used input
    glfwPollEvents();

    // Clear the screen
    glClear(GL_COLOR_BUFFER_BIT);

    // Prepare the shaders to draw
    glUseProgram(shader);

    // Bind the triangle texture
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, triangle_tex);
    glUniform1i(texLocation, 0);
    // Draw the triangle
    glBindVertexArray(triangle_VAO);
    glDrawArrays(GL_TRIANGLES, 0, 3);

    // Bind the rectangle texture
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, rectangle_tex);
    glUniform1i(texLocation, 0);
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
