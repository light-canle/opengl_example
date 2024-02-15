#version 330 core
// 최종 출력색, vertex shader에서 받은 텍스쳐
out vec4 fragColor;
in vec2 texCoord;

// G-Buffer의 데이터를 텍스쳐 형태로 받는다.
uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gAlbedoSpec;

// 빛의 위치와 색상 - attenuation(감쇠)이 없는 point light 형태로 구현
struct Light {
    vec3 position;
    vec3 color;
};

// 32개의 빛을 담은 배열
const int NR_LIGHTS = 32;
uniform Light lights[NR_LIGHTS];

// 카메라 위치
uniform vec3 viewPos;

void main() {
    // retrieve data from G-buffer
    vec3 fragPos = texture(gPosition, texCoord).rgb;
    vec3 normal = texture(gNormal, texCoord).rgb;
    vec3 albedo = texture(gAlbedoSpec, texCoord).rgb;
    float specular = texture(gAlbedoSpec, texCoord).a;
    // then calculate lighting as usual
    // G-Buffer 데이터를 받아 원래 빛을 계산하는 공식을 쓴다.
    vec3 lighting = albedo * 0.1; // hard-coded ambient component(주변광)
    vec3 viewDir = normalize(viewPos - fragPos);
    // 32개의 빛에 대해 빛의 방향을 계산해서 분산광을 계산한다.
    for(int i = 0; i < NR_LIGHTS; ++i) {
        // diffuse
        vec3 lightDir = normalize(lights[i].position - fragPos);
        vec3 diffuse = max(dot(normal, lightDir), 0.0) * albedo * lights[i].color;
        lighting += diffuse;
    }
    // 최종 색 반환
    fragColor = vec4(lighting, 1.0);
}