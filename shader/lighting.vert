#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;

uniform mat4 transform;
uniform mat4 modelTransform;

out vec3 normal;
out vec2 texCoord;
out vec3 position;

void main() {
    gl_Position = transform * vec4(aPos, 1.0); // local -> canonical (화면상 좌표)
    normal = (transpose(inverse(modelTransform)) * vec4(aNormal, 0.0)).xyz; // normal vec (벡터이므로 inverse transpose를 한다.)
    texCoord = aTexCoord; // 텍스쳐 좌표
    position = (modelTransform * vec4(aPos, 1.0)).xyz; // local -> world (월드 절대 좌표)
}