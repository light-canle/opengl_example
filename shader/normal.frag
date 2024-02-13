#version 330 core

in vec2 texCoord;
in vec3 position;
in vec3 normal;
in vec3 tangent;
out vec4 fragColor;

// 카메라와 빛의 위치
uniform vec3 viewPos;
uniform vec3 lightPos;

// 텍스쳐와 normal map
uniform sampler2D diffuse;
uniform sampler2D normalMap;

void main() {
    // 원래 텍스쳐에서 색을 들고옴
    vec3 texColor = texture(diffuse, texCoord).xyz;
    // normal map에서 값을 들고온 뒤, 범위를 [-1, 1]로 조정함
    vec3 texNorm = texture(normalMap, texCoord).xyz * 2.0 - 1.0;
    // normal, tangent, binormal vector를 구함
    vec3 N = normalize(normal);
    vec3 T = normalize(tangent);
    vec3 B = cross(N, T);
    // TBN matrix
    mat3 TBN = mat3(T, B, N);
    // 원래 normal에다가 TBN matrix를 곱해서 월드 좌표 에서의 normal vector를 구함
    vec3 pixelNorm = normalize(TBN * texNorm);
    // 원래 텍스쳐에서 색을 어둡게 한 것을 주변광으로 사용
    vec3 ambient = texColor * 0.2;

    // diffuse 계산
    vec3 lightDir = normalize(lightPos - position);
    float diff = max(dot(pixelNorm, lightDir), 0.0);
    vec3 diffuse = diff * texColor * 0.8;

    // reflect 계산
    vec3 viewDir = normalize(viewPos - position);
    vec3 reflectDir = reflect(-lightDir, pixelNorm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    vec3 specular = spec * vec3(0.5);

    // 최종 색 계산
    fragColor = vec4(ambient + diffuse + specular, 1.0);
}