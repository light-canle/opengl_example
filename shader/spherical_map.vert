#version 330 core
// skybox.vert와 거의 비슷함
layout (location = 0) in vec3 aPos;
out vec3 localPos;
uniform mat4 transform;

void main() {
    gl_Position = transform * vec4(aPos, 1.0);
    localPos = aPos;
}