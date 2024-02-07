#version 330 core

out vec4 fragColor;

in vec3 normal;
in vec3 position;

uniform vec3 cameraPos;
uniform samplerCube skybox;

void main() {
    // 눈으로 바라보는 벡터
    vec3 I = normalize(position - cameraPos);
    // 반사 벡터
    vec3 R = reflect(I, normalize(normal));
    fragColor = vec4(texture(skybox, R).rgb, 1.0);
}