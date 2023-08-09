#include <glutils.hpp>
#include <iostream>
#include <string>
#define STB_IMAGE_IMPLEMENTATION
#include "flycamera.hpp"
#include "shader.hpp"
#include "stb/stb_image.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// Config for the turn animation speed
const int MIN_SPEED = 2;
const int MAX_SPEED = 5;
// Camera movement speed with user input
const float CAMERA_SPEED = 3.0f;

const float WIN_WIDTH = 800.0f;
const float WIN_HEIGHT = 600.0f;

// Auxiliary variables of the mouse controller
bool firstMouse = true;
float lastX = WIN_WIDTH / 2.0;
float lastY = WIN_HEIGHT / 2.0;

// Set camera initial view config
const glm::vec3 cameraPos = glm::vec3(0.0f, 1.0f, 8.0f);
const glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
// Create the camera controller
Camera camera = Camera(cameraPos, cameraUp);
// Camera Field Of View
float fov = 45.0f;

// Lighting
glm::vec3 light_position = glm::vec3(0.0f, 0.5f, 0.0f);
glm::vec3 lightCubeColor = glm::vec3(0.5f, 0.5f, 1.0f);

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

void set_up_roof() {
  // Triangle vertices data
  // Format: postion(x, y, z), texCoord(x, y)
  constexpr float vertices[25] = {
      -0.5f, 0.0f, 0.5f,  0.0f, 0.0f, // Left-front
      0.5f,  0.0f, 0.5f,  1.0f, 0.0f, // Right-front
      0.0f,  0.5f, 0.0f,  0.5f, 1.0f, // Top-center
      -0.5f, 0.0f, -0.5f, 1.0f, 0.0f, // Left-back
      0.5f,  0.0f, -0.5f, 0.0f, 0.0f, // Right-back
  };
  // Order to draw the unique vertices to form the roof pyramid
  constexpr GLuint indices[12] = {
      0, 2, 1, // Front
      1, 2, 4, // Right
      4, 2, 3, // Back
      3, 2, 0  // Left
  };
  // Prepare the Element Buffer Object for `index drawing`
  GLuint EBO;
  glGenBuffers(1, &EBO);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
  // Push the vertex indexes into the buffer
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices,
               GL_STATIC_DRAW);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
  // Prepare the buffer to sed the vertex data
  GLuint VBO;
  glGenBuffers(1, &VBO);
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  // Push the vertex data into the buffer
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
  // Prepare the vertex position attribute
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)0);
  glEnableVertexAttribArray(0);
  // Prepare the texture coordinates attribute
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float),
                        (void *)(3 * sizeof(float)));
  glEnableVertexAttribArray(1);
}

void set_up_walls() {
  // Rectangle walls unique vertices data
  // Format: postion(x, y, z), texCoord(x, y)
  constexpr float vertices[40] = {
      0.5f,  0.0f,  0.5f,  1.0f, 1.0f, // Top-right-front
      0.5f,  -0.5f, 0.5f,  1.0f, 0.0f, // Bottom-right-front
      -0.5f, -0.5f, 0.5f,  0.0f, 0.0f, // Bottom-left-front
      -0.5f, 0.0f,  0.5f,  0.0f, 1.0f, // Top-left-front
      0.5f,  0.0f,  -0.5f, 0.0f, 1.0f, // Top-right-back
      0.5f,  -0.5f, -0.5f, 0.0f, 0.0f, // Bottom-right-back
      -0.5f, -0.5f, -0.5f, 1.0f, 0.0f, // Bottom-left-back
      -0.5f, 0.0f,  -0.5f, 1.0f, 1.0f  // Top-left-back
  };
  // Order to draw the unique vertices to form the walls cube
  constexpr GLuint indices[24] = {
      0, 1, 3, // Top-front triangle
      1, 2, 3, // Bottom-front triangle
      4, 5, 7, // Top-back triangle
      5, 6, 7, // Bottom-back triangle
      0, 4, 1, // Top-right triangle
      1, 4, 5, // Bottom-right triangle
      3, 2, 7, // Top-left triangle
      2, 6, 7  // Bottom-left triangle
  };
  // Prepare the Element Buffer Object for `index drawing`
  GLuint EBO;
  glGenBuffers(1, &EBO);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
  // Push the vertex indexes into the buffer
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices,
               GL_STATIC_DRAW);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
  // Prepare the vertex buffer for the unique vertices data
  GLuint VBO;
  glGenBuffers(1, &VBO);
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  // Push the vertex data into the buffer
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
  // Prepare the vertex position attribute
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)0);
  glEnableVertexAttribArray(0);
  // Prepare the texture coordinates attribute
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float),
                        (void *)(3 * sizeof(float)));
  glEnableVertexAttribArray(1);
}

void set_up_light() {
  // Vertices to create a cube
  float vertices[108] = {
      -0.5f, -0.5f, -0.5f, 0.5f,  -0.5f, -0.5f, 0.5f,  0.5f,  -0.5f, 0.5f,
      0.5f,  -0.5f, -0.5f, 0.5f,  -0.5f, -0.5f, -0.5f, -0.5f, -0.5f, -0.5f,
      0.5f,  0.5f,  -0.5f, 0.5f,  0.5f,  0.5f,  0.5f,  0.5f,  0.5f,  0.5f,
      -0.5f, 0.5f,  0.5f,  -0.5f, -0.5f, 0.5f,  -0.5f, 0.5f,  0.5f,  -0.5f,
      0.5f,  -0.5f, -0.5f, -0.5f, -0.5f, -0.5f, -0.5f, -0.5f, -0.5f, -0.5f,
      0.5f,  -0.5f, 0.5f,  0.5f,  0.5f,  0.5f,  0.5f,  0.5f,  0.5f,  -0.5f,
      0.5f,  -0.5f, -0.5f, 0.5f,  -0.5f, -0.5f, 0.5f,  -0.5f, 0.5f,  0.5f,
      0.5f,  0.5f,  -0.5f, -0.5f, -0.5f, 0.5f,  -0.5f, -0.5f, 0.5f,  -0.5f,
      0.5f,  0.5f,  -0.5f, 0.5f,  -0.5f, -0.5f, 0.5f,  -0.5f, -0.5f, -0.5f,
      -0.5f, 0.5f,  -0.5f, 0.5f,  0.5f,  -0.5f, 0.5f,  0.5f,  0.5f,  0.5f,
      0.5f,  0.5f,  -0.5f, 0.5f,  0.5f,  -0.5f, 0.5f,  -0.5f};
  GLuint VBO;
  glGenBuffers(1, &VBO);
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
  glEnableVertexAttribArray(0);
}

void mouse_callback(GLFWwindow *window, double xposIn, double yposIn) {
  float xpos = static_cast<float>(xposIn);
  float ypos = static_cast<float>(yposIn);

  if (firstMouse) {
    lastX = xpos;
    lastY = ypos;
    firstMouse = false;
  }

  float xoffset = xpos - lastX;
  float yoffset =
      lastY - ypos; // reversed since y-coordinates go from bottom to top
  lastX = xpos;
  lastY = ypos;

  camera.ProcessMouseMovement(xoffset, yoffset);
}

void scroll_callback(GLFWwindow *window, double xoffset, double yoffset) {
  fov -= (float)yoffset;
  if (fov < 1.0f)
    fov = 1.0f;
  if (fov > 45.0f)
    fov = 45.0f;
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
  GLFWwindow *window =
      glfwCreateWindow(WIN_WIDTH, WIN_HEIGHT, "OpenGL Sandbox", NULL, NULL);
  if (!window) {
    glfwTerminate();
    exit(EXIT_FAILURE);
  }
  // Capture the mouse when focusing the window
  glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

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

  // Set the color to clear the screen
  glClearColor(0.25f, 0.5f, 0.75f, 1.0f);

  // Enable Z-buffer
  glEnable(GL_DEPTH_TEST);

  Shader base_shader = Shader("../../src/shaders/lighting.vert",
                              "../../src/shaders/lighting.frag");

  // Prepare the roof texture
  GLuint roof_tex = texture_setup("../../textures/roof.png");
  // Store the roof vertices config in a Vertex Array Object
  GLuint roof_VAO;
  glGenVertexArrays(1, &roof_VAO);
  glBindVertexArray(roof_VAO);
  set_up_roof();

  // Prepare the walls texture
  GLuint wall_tex = texture_setup("../../textures/container.jpg");
  // Store the walls vertices config in a Vertex Array Object
  GLuint walls_VAO;
  glGenVertexArrays(1, &walls_VAO);
  glBindVertexArray(walls_VAO);
  set_up_walls();

  // Store the walls vertices config in a Vertex Array Object
  GLuint light_VAO;
  glGenVertexArrays(1, &light_VAO);
  glBindVertexArray(light_VAO);
  set_up_light();

  glm::vec3 house_positions[6] = {
      glm::vec3(2.0f, 0.0f, -1.0f),  glm::vec3(-1.0f, 0.0f, 0.5f),
      glm::vec3(0.9f, 0.0f, 1.0f),   glm::vec3(0.7f, 0.0f, -3.0f),
      glm::vec3(-2.0f, 0.0f, -2.0f), glm::vec3(-0.8f, 0.0f, -6.0f)};

  // Get uniform variables locations to update them in the render loop
  GLuint useTexLoc = glGetUniformLocation(base_shader.ID, "useTexture");
  GLuint texLoc = glGetUniformLocation(base_shader.ID, "baseTexture");
  GLuint lightColorLoc = glGetUniformLocation(base_shader.ID, "lightColor");
  GLuint objectColorLoc = glGetUniformLocation(base_shader.ID, "objectColor");
  GLuint modelLoc = glGetUniformLocation(base_shader.ID, "model");
  GLuint viewLoc = glGetUniformLocation(base_shader.ID, "view");
  GLuint projectionLoc = glGetUniformLocation(base_shader.ID, "projection");

  // Set mouse handling callback
  glfwSetCursorPosCallback(window, mouse_callback);
  glfwSetScrollCallback(window, scroll_callback);

  // Keep track of the elapsed time to control movement speed
  float lastFrameTime = 0.0f;

  while (!glfwWindowShouldClose(window)) {
    // Read used input
    glfwPollEvents();

    // Clear the screen
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Prepare the shaders to draw
    base_shader.use();

    // Compute elapsed time between frames
    const float currentFrameTime = glfwGetTime();
    const float deltaTime = currentFrameTime - lastFrameTime;
    lastFrameTime = currentFrameTime;

    const float cameraSpeed = CAMERA_SPEED * deltaTime;
    // Process user input to move the camera
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
      camera.ProcessKeyboard(CameraMovement::FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
      camera.ProcessKeyboard(CameraMovement::BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
      camera.ProcessKeyboard(CameraMovement::LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
      camera.ProcessKeyboard(CameraMovement::RIGHT, deltaTime);

    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
      break; // Exit the program loop

    // Create the LooAt matrix for the camera
    glm::mat4 view = camera.GetViewMatrix();

    // Create the perspective projection matrix
    glm::mat4 projection = glm::perspective(
        glm::radians(fov), WIN_WIDTH / WIN_HEIGHT, 0.1f, 100.0f);

    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

    glUniform1ui(useTexLoc, 1);
    glUniform3f(lightColorLoc, 1.0f, 1.0f, 1.0f);
    // Set the color for the houses
    glUniform3f(objectColorLoc, 1.0f, 1.0f, 1.0f);

    int speed_idx = 0;
    bool invert_turn = false;
    // Draw each house in its corresponding postion using the model transform
    for (glm::vec3 &house_pos : house_positions) {
      // Initialize the transform matrix with the identity matrix
      glm::mat4 model = glm::mat4(1.0f);
      // Apply translation between rotations
      model = glm::translate(model, house_pos);
      // Apply rotation over Y-axis using the elapsed time
      const float rotation =
          (speed_idx % MAX_SPEED + MIN_SPEED) * glfwGetTime();
      model = glm::rotate(model, invert_turn ? -rotation : rotation,
                          glm::vec3(0.0f, 1.0f, 0.0f));
      speed_idx++;
      invert_turn = !invert_turn;
      // Set transform data for the shader
      glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

      // Bind the roof texture
      glActiveTexture(GL_TEXTURE0);
      glBindTexture(GL_TEXTURE_2D, roof_tex);
      glUniform1i(texLoc, 0);
      // Draw the roof
      glBindVertexArray(roof_VAO);
      glDrawElements(GL_TRIANGLES, 12, GL_UNSIGNED_INT, 0);

      // Bind the walls texture
      glActiveTexture(GL_TEXTURE0);
      glBindTexture(GL_TEXTURE_2D, wall_tex);
      glUniform1i(texLoc, 0);
      // Draw the walls
      glBindVertexArray(walls_VAO);
      glDrawElements(GL_TRIANGLES, 24, GL_UNSIGNED_INT, 0);
    }

    // Disable textures. The light cube only uses color
    glUniform1ui(useTexLoc, 0);
    // Set the color for the light cube
    glUniform3fv(objectColorLoc, 1, glm::value_ptr(lightCubeColor));
    // Set up the light postion and scale
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, light_position);
    model = glm::scale(model, glm::vec3(0.5f, 0.5f, 0.5f));
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
    // Draw the light cube
    glBindVertexArray(light_VAO);
    glDrawArrays(GL_TRIANGLES, 0, 36);

    // Display the updated rendered data
    glfwSwapBuffers(window);
  }

  glDeleteProgram(base_shader.ID);
  glfwTerminate();
  return 0;
}
