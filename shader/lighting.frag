#version 330 core
in vec3 noraml;
in vec2 texCoord;
out vec4 fragColor;

uniform vec3 lightColor; // 광원의 색상
uniform vec3 objectColor; // 물체의 색상
uniform float ambientStrength; // 주변광의 세기(0~1)

void main() {
  vec3 ambient = ambientStrength * lightColor;
  vec3 result = ambient * objectColor;
  fragColor = vec4(result, 1.0);
}