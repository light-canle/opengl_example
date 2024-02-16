#version 330 core

// AO값만 출력하기 위해 out 변수가 float이다.
out float fragColor;

in vec2 texCoord;

uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D texNoise;

uniform mat4 view;
uniform mat4 projection; // Screen-space 계산을 위해 필요

uniform vec2 noiseScale;
uniform float radius; // 샘플들을 저장하는 반구의 반지름

// random samples
const int KERNEL_SIZE = 64;
const float BIAS = 0.025; // acne 방지를 위한 편향값
uniform vec3 samples[KERNEL_SIZE]; // 반구 내 임의의 위치를 담은 배열


void main() {
    vec4 worldPos = texture(gPosition, texCoord);
    if (worldPos.w <= 0.0f)
        discard;

    // view space (화면 좌표)를 기준으로 한 픽셀의 좌표와 법선 벡터를 가져옴
    vec3 fragPos = (view * vec4(worldPos.xyz, 1.0)).xyz;
    vec3 normal = (view * vec4(texture(gNormal, texCoord).xyz, 0.0)).xyz;
    // 텍스쳐에서 랜덤 회전 벡터를 가져옴
    vec3 randomVec = texture(texNoise, texCoord * noiseScale).xyz;
    
    // Gram-Schmidt 과정을 이용해 randomVec를 normal vector에 수직한 평면에 내린 정사영인 tangent 벡터를 구함
    vec3 tangent = normalize(randomVec - normal * dot(randomVec, normal));
    vec3 binormal = cross(normal, tangent);
    // tangent space를 view space로 변환하는 행렬
    mat3 TBN = mat3(tangent, binormal, normal);

    float occlusion = 0.0;
    for (int i = 0; i < KERNEL_SIZE; i++) {
        // 픽셀 위치에서 반지름이 radius인 반구 내 임의의 위치(샘플)를 가져온다. 
        vec3 sample = fragPos + TBN * samples[i] * radius;
        // 샘플의 위치가 screen space 상에서 어딘지를 구한다.
        vec4 screenSample = projection * vec4(sample, 1.0);
        screenSample.xyz /= screenSample.w;
        screenSample.xyz = screenSample.xyz * 0.5 + 0.5; // 0 ~ 1사이 값으로 변환
    
        // 화면 상 좌표에서 픽셀의 z값과 샘플의 z값을 비교해서 차폐의 여부를 검증한다.
        float sampleDepth = (view * texture(gPosition, screenSample.xy)).z;
        // 반구 밖에 있는 샘플의 영향력을 줄이기 위한 값(멀어질 수록 값이 줄어듦)
        float rangeCheck = smoothstep(0.0, 1.0, radius / abs(fragPos.z - sampleDepth));
        occlusion += (sampleDepth >= sample.z + BIAS ? 1.0 : 0.0) * rangeCheck;
    }

    fragColor = 1.0 - occlusion / KERNEL_SIZE;
}