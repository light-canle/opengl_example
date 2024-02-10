// spot light를 위한 frag shader
#version 330 core
in vec3 normal;
in vec2 texCoord;
in vec3 position;
out vec4 fragColor;

uniform vec3 viewPos; // 카메라가 보는 위치
uniform int blinn; // blinn-phong shading on/off

// 빛 정보를 담는 구조체
struct Light {
    vec3 position; // 빛의 위치
    vec3 direction; // 빛이 발사되는 방향
    vec2 cutoff; // 빛이 100% 강도로 비춰지는 inner_angle과 0~100% 사이로 보간되는 경계각 outer_angle로 이루어진 vec2
    vec3 attenuation; // 빛의 감쇠 : 세 상수 (k_c, k_l, k_q)로 구성된 vec3
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
    float dist = length(light.position - position); // 빛과 물체 사이 거리
    vec3 distPoly = vec3(1.0, dist, dist*dist); // 상수항, 1차항, 2차항
    // 1.0 / (1.0 * k_c + d * k_l + d^2 + k_q)
    float attenuation = 1.0 / dot(distPoly, light.attenuation); // 거리에 따른 빛의 감쇠 공식
    vec3 lightDir = (light.position - position) / dist; // 빛의 방향

    vec3 result = ambient; // 최종 색깔(주변광 우선 적용)
    // 각도
    float theta = dot(lightDir, normalize(-light.direction));
    // 보간된 빛의 세기
    // 0 ~ inner : 100%, inner ~ outer : 0 ~ 100%(아래 식에 의해 보간), outer 초과 : 0%
    float intensity = clamp((theta - light.cutoff[1]) / (light.cutoff[0] - light.cutoff[1]), 0.0, 1.0);
    // outer_angle보다 작은 경우에만 빛 계산, 아니면 주변광만 적용
    if (intensity > 0.0) {
        vec3 pixelNorm = normalize(normal);
        float diff = max(dot(pixelNorm, lightDir), 0.0);
        vec3 diffuse = diff * texColor * light.diffuse;

        vec3 specColor = texture2D(material.specular, texCoord).xyz;
        float spec = 0.0f;
        // Phong shading
        if (blinn == 0) {
            vec3 viewDir = normalize(viewPos - position);
            vec3 reflectDir = reflect(-lightDir, pixelNorm);
            spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
        }
        // Blinn-Phong shading
        else {
            vec3 viewDir = normalize(viewPos - position);
            vec3 halfDir = normalize(lightDir + viewDir);
            spec = pow(max(dot(halfDir, pixelNorm), 0.0), material.shininess);
        }
        vec3 specular = spec * specColor * light.specular;
        result += (diffuse + specular) * intensity;
    }
    result *= attenuation;
    fragColor = vec4(result, 1.0);
    
    // z buffer 값을 시각화하기 위한 테스트 코드
    // fragColor = vec4(vec3(gl_FragCoord.z), 1.0);
}