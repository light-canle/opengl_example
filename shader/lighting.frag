#version 330 core
in vec3 normal;
in vec3 position;
in vec2 texCoord;
out vec4 fragColor;

uniform vec3 lightPos; // 광원의 위치
uniform vec3 lightColor; // 광원의 색상
uniform vec3 objectColor; // 물체의 색상
uniform float ambientStrength; // 주변광의 세기(0~1)

void main() {
    vec3 ambient = ambientStrength * lightColor;
    vec3 lightDir = normalize(lightPos - position); // 빛의 방향
    vec3 pixelNorm = normalize(normal); // 법선 벡터 정규화
    vec3 diffuse = max(dot(pixelNorm, lightDir), 0.0) * lightColor; // 분산광
    vec3 result = (ambient + diffuse) * objectColor; // 최종적인 색상
    fragColor = vec4(result, 1.0);
}