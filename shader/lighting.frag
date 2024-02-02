// directional light를 위한 frag shader
#version 330 core
in vec3 normal;
in vec2 texCoord;
in vec3 position;
out vec4 fragColor;

uniform vec3 viewPos; // 카메라가 보는 위치

// 빛 정보를 담는 구조체
struct Light {
    vec3 direction; // 빛의 방향
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};
uniform Light light;

// 물체의 재질 정보를 담는 구조체
// material의 diffuse와 ambient는 동일한 것으로 간주
struct Material {
    sampler2D diffuse; // 텍스쳐를 위한 sampler2D
    sampler2D specular; // 텍스쳐의 부분들마다 재질을 다르게 설정할 수 있게 함
    float shininess; // 표면의 반짝임 정도
};
uniform Material material;

void main() {
    // 주변광
    vec3 texColor = texture2D(material.diffuse, texCoord).xyz;
    vec3 ambient = texColor * light.ambient;
    // 분산광
    vec3 lightDir = normalize(-light.direction); // 빛의 방향
    vec3 pixelNorm = normalize(normal);
    float diff = max(dot(pixelNorm, lightDir), 0.0);
    vec3 diffuse = diff * texColor * light.diffuse;
    // 반사광
    vec3 specColor = texture2D(material.specular, texCoord).xyz;
    vec3 viewDir = normalize(viewPos - position); // 보는 방향
    vec3 reflectDir = reflect(-lightDir, pixelNorm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = spec * specColor * light.specular;
    // 최종 색깔
    vec3 result = ambient + diffuse + specular;
    fragColor = vec4(result, 1.0);
}