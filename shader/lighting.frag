#version 330 core
in vec3 normal;
in vec3 position;
in vec2 texCoord;
out vec4 fragColor;

uniform vec3 viewPos; // 카메라가 보는 위치

// 빛 정보를 담는 구조체
struct Light {
    vec3 position; // 빛의 위치
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};
uniform Light light;

// 물체의 재질 정보를 담는 구조체
struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess; // 표면의 반짝임 정도
};
uniform Material material;


void main() {
    // 주변광
    vec3 ambient = material.ambient * light.ambient;
    // 분산광
    vec3 lightDir = normalize(light.position - position); // 빛의 방향
    vec3 pixelNorm = normalize(normal);
    float diff = max(dot(pixelNorm, lightDir), 0.0);
    vec3 diffuse = diff * material.diffuse * light.diffuse;
    // 반사광
    vec3 viewDir = normalize(viewPos - position); // 보는 방향
    vec3 reflectDir = reflect(-lightDir, pixelNorm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = spec * material.specular * light.specular;
    // 최종 색깔
    vec3 result = ambient + diffuse + specular;
    fragColor = vec4(result, 1.0);
}