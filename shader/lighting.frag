#version 330 core
in vec3 normal;
in vec3 position;
in vec2 texCoord;
out vec4 fragColor;

uniform vec3 lightPos; // 광원의 위치
uniform vec3 lightColor; // 광원의 색상
uniform vec3 objectColor; // 물체의 색상
uniform float ambientStrength; // 주변광의 세기(0~1)

uniform float specularStrength; // 반사광의 세기
uniform float specularShininess; // 반사광의 양(면적)
uniform vec3 viewPos; // 카메라가 보는 위치

void main() {
    // 주변광
    vec3 ambient = ambientStrength * lightColor;
    // 분산광
    vec3 lightDir = normalize(lightPos - position); // 빛의 방향
    vec3 pixelNorm = normalize(normal); // 법선 벡터 정규화
    vec3 diffuse = max(dot(pixelNorm, lightDir), 0.0) * lightColor; // 분산광
    // 반사광
    vec3 viewDir = normalize(viewPos - position); // 보는 방향
    vec3 reflectDir = reflect(-lightDir, pixelNorm); // 반사하는 방향의 벡터
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), specularShininess); // 반사광의 양
    vec3 specular = specularStrength * spec * lightColor;
    // 최종적인 색상
    vec3 result = (ambient + diffuse + specular) * objectColor;
    fragColor = vec4(result, 1.0);
}