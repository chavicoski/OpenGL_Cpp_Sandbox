#version 330 core

in vec2 texCoord;

out vec4 screenColor;

uniform bool useTexture;
uniform sampler2D baseTexture;
uniform vec3 lightColor;
uniform vec3 objectColor;

void main() {
  if (useTexture)
    screenColor = texture(baseTexture, texCoord) * vec4(lightColor * objectColor, 1.0);
  else
    screenColor = vec4(lightColor * objectColor, 1.0);
}