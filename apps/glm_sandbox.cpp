#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>

int main() {
  /*
   * Simple translation
   */
  glm::vec4 vec(1.0f, 0.0f, 0.0f, 1.0f);   // Target vector to transform
  glm::mat4 transform = glm::mat4(1.0f);   // Identity matrix
  glm::vec3 translation(1.0f, 1.0f, 0.0f); // Translation vector
  // Create the translation matrix
  transform = glm::translate(transform, translation);
  // Apply the translation
  vec = transform * vec;
  printf("(x=%.1f, y=%.1f, z=%.1f)\n", vec.x, vec.y, vec.z);

  /*
   * Scale and rotate
   */
  transform = glm::mat4(1.0f);                 // Reset to Identity matrix
  glm::vec3 z_unit = glm::vec3(0.0, 0.0, 1.0); // Z-axis unit vector
  // Add rotate transform over the Z-axis
  transform = glm::rotate(transform, glm::radians(90.0f), z_unit);
  // Add uniform scale transform
  transform = glm::scale(transform, glm::vec3(0.5, 0.5, 0.5));
  // Apply the transform. Note that scale is applied before rotation
  vec = transform * vec;
  printf("(x=%.1f, y=%.1f, z=%.1f)\n", vec.x, vec.y, vec.z);
  return 0;
}