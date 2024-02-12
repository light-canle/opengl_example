#version 330 core

out vec4 fragColor;

in VS_OUT {
    vec3 fragPos;
    vec3 normal;
    vec2 texCoord;
    vec4 fragPosLight;
} fs_in;

uniform vec3 viewPos;
struct Light {
    vec3 position;
    vec3 direction;
    vec2 cutoff;
    vec3 attenuation;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};
uniform Light light;

struct Material {
    sampler2D diffuse;
    sampler2D specular;
    float shininess;
};
uniform Material material;
uniform int blinn;
uniform sampler2D shadowMap; // depth map을 입력받기 위한 sampler2D

// 해당 칸의 그림자의 세기를 계산해서 반환(0~1)
float ShadowCalculation(vec4 fragPosLight, vec3 normal, vec3 lightDir) {
    // perform perspective divide
    vec3 projCoords = fragPosLight.xyz / fragPosLight.w;
    // transform to [0,1] range ([-1, 1]을 [0, 1]로 변경)
    projCoords = projCoords * 0.5 + 0.5;
    // get closest depth value from light’s perspective (using
    // [0,1] range fragPosLight as coords) - 쉐도우 맵에서 부터 depth 값을 가져온다.
    float closestDepth = texture(shadowMap, projCoords.xy).r;
    // get depth of current fragment from light’s perspective - 빛의 위치를 기준으로 depth를 구함
    float currentDepth = projCoords.z;
    // check whether current frag pos is in shadow - 둘을 비교해서 그림자의 여부를 결정
    float bias = max(0.02 * (1.0 - dot(normal, lightDir)), 0.001); // bias는 0.001 ~ 0.02 사이의 가변값이 됨
    float shadow = currentDepth - bias > closestDepth ? 1.0 : 0.0;
    return shadow;
}

void main() {
    // 일반적인 빛 shader 내용과 비슷함
    vec3 texColor = texture2D(material.diffuse, fs_in.texCoord).xyz;
    vec3 ambient = texColor * light.ambient;

    float dist = length(light.position - fs_in.fragPos);
    vec3 distPoly = vec3(1.0, dist, dist*dist);
    float attenuation = 1.0 / dot(distPoly, light.attenuation);
    vec3 lightDir = (light.position - fs_in.fragPos) / dist;

    vec3 result = ambient;
    float theta = dot(lightDir, normalize(-light.direction));
    float intensity = clamp((theta - light.cutoff[1]) / (light.cutoff[0] - light.cutoff[1]), 0.0, 1.0);

    if (intensity > 0.0) {
        vec3 pixelNorm = normalize(fs_in.normal);
        float diff = max(dot(pixelNorm, lightDir), 0.0);
        vec3 diffuse = diff * texColor * light.diffuse;

        vec3 specColor = texture2D(material.specular, fs_in.texCoord).xyz;
        float spec = 0.0;
        if (blinn == 0) {
            vec3 viewDir = normalize(viewPos - fs_in.fragPos);
            vec3 reflectDir = reflect(-lightDir, pixelNorm);
            spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
        }
        else {
            vec3 viewDir = normalize(viewPos - fs_in.fragPos);
            vec3 halfDir = normalize(lightDir + viewDir);
            spec = pow(max(dot(halfDir, pixelNorm), 0.0), material.shininess);
        }
        vec3 specular = spec * specColor * light.specular;
        // 거기에 추가적으로 그림자 세기를 계산해 준다.
        // 0이면 그림자가 없는 것이고, 1이면 완전히 그림자가 져서 검게 보인다.
        float shadow = ShadowCalculation(fs_in.fragPosLight, pixelNorm, lightDir);

        result += (diffuse + specular) * intensity * (1.0 - shadow);
    }

    result *= attenuation;
    fragColor = vec4(result, 1.0);
}