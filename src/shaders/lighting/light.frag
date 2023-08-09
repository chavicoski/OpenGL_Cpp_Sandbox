#version 330 core

out vec4 screenColor;

uniform vec3 objectColor;

void main() {
  screenColor = vec4(objectColor, 1.0);
}