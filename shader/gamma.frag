#version 330 core
in vec4 vertexColor;
in vec2 texCoord;
out vec4 fragColor;

uniform sampler2D tex;
uniform float gamma;

void main() {
    vec4 pixel = texture(tex, texCoord);
    // Post-processing
    // 픽셀 색깔에 각각 gamma 값을 제곱해서 반환
    // gamma가 1이면 원래 색, 1보다 작으면 원래보다 밝게, 1보다 크면 원래보다 어둡게 나온다.
    fragColor = vec4(pow(pixel.rgb, vec3(gamma)), 1.0);
}