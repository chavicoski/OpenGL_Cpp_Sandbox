#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>

int main() {
  glm::vec4 vec(1.0f, 0.0f, 0.0f, 1.0f);   // Target vector to transform
  glm::mat4 transform = glm::mat4(1.0f);   // Identity matrix
  glm::vec3 translation(1.0f, 1.0f, 0.0f); // Translation vector
  // Create the translation matrix
  transform = glm::translate(transform, translation);
  // Apply the translation
  vec = transform * vec;
  printf("(x=%.1f, y=%.1f, z=%.1f)\n", vec.x, vec.y, vec.z);
  return 0;
}