#version 330 core
in vec4 vertexColor;
in vec2 texCoord;
out vec4 fragColor;

uniform sampler2D tex;

void main() {
    // 텍스쳐를 그대로 적용
    vec4 pixel = texture(tex, texCoord);
    // alpha가 너무 낮으면 해당 픽셀은 그리지 않음
    if (pixel.a < 0.01){
        discard;
    }
    fragColor = pixel;
}