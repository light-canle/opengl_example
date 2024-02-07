#version 330 core
out vec4 fragColor;
in vec3 texCoord;
// 스카이 박스
uniform samplerCube skybox;

void main() {
    // 3D 좌표의 방향의 벡터와 스카이 박스가 만나는 부분의 픽셀로 그림을 그림
    fragColor = texture(skybox, texCoord);
}