#version 330 core

// G-Buffer를 위한 3개의 output - 3개의 color attachment를 반환
layout (location = 0) out vec4 gPosition; // 위치
layout (location = 1) out vec4 gNormal; // 법선
layout (location = 2) out vec4 gAlbedoSpec; // 기본 배색(RGB)과 반사광(A)

// vertex shader로부터의 input
in vec3 position;
in vec3 normal;
in vec2 texCoord;

struct Material {
    sampler2D diffuse; // 기본 배색
    sampler2D specular; // 반사광
};
uniform Material material;

void main() {
    // store the fragment position vector in the first gbuffer texture - fragment의 위치를 저장
    gPosition = vec4(position, 1.0);
    // also store the per-fragment normals into the gbuffer - fragment의 법선 벡터 저장
    gNormal = vec4(normalize(normal), 1.0);
    // and the diffuse per-fragment color - 기본 배색을 저장
    gAlbedoSpec.rgb = texture(material.diffuse, texCoord).rgb;
    // store specular intensity in gAlbedoSpec’s alpha component - 반사 정도를 저장
    gAlbedoSpec.a = texture(material.specular, texCoord).r;
}