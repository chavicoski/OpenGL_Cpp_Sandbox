#version 330 core

in vec3 fragColor;
in vec2 texCoord;

out vec4 screenColor;

uniform sampler2D baseTexture;

void main() {
  screenColor = texture(baseTexture, texCoord) * vec4(fragColor, 1.0);
}