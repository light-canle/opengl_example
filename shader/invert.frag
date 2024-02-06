#version 330 core
in vec4 vertexColor;
in vec2 texCoord;
out vec4 fragColor;

uniform sampler2D tex;

void main() {
    vec4 pixel = texture(tex, texCoord);
    // Post-processing
    // 픽셀 색을 뒤집어 반전색을 반환
    fragColor = vec4(1.0 - pixel.rgb, 1.0);
}