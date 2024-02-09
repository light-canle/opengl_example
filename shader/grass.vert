#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 2) in vec2 aTexCoord;
// 위치와 회전각을 담은 offset - 인스턴스 마다 값이 다름
layout (location = 3) in vec3 aOffset;
out vec2 texCoord;

uniform mat4 transform;

void main() {
    // 회전각의 사인, 코사인
    float c = cos(aOffset.y);
    float s = sin(aOffset.y);
    // 쉐이더에서 직접 행렬을 계산한다. (Translation + Rotation)
    mat4 offsetMat = mat4(
        c, 0.0, -s, 0.0,
        0.0, 1.0, 0.0, 0.0,
        s, 0.0, c, 0.0,
        aOffset.x, 0.0, aOffset.z, 1.0);
    gl_Position = transform * offsetMat * vec4(aPos, 1.0);
    texCoord = aTexCoord;
}