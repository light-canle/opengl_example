#version 330 core

in vec2 texCoord;
out vec4 fragColor;

uniform sampler2D tex;

void main() {
    // 받은 텍스쳐를 그대로 입힌다.
    vec4 pixel = texture(tex, texCoord);
    // 알파가 0.05 이하인 픽셀은 그리지 않음
    if (pixel.a < 0.05)
        discard;
    fragColor = pixel;
}