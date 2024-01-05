#version 330 core
in vec4 vertexColor;
in vec2 texCoord;
out vec4 fragColor;

uniform sampler2D tex;
uniform sampler2D tex2;

void main() {
    // 텍스쳐 블랜딩
    fragColor = texture(tex, texCoord) * 0.8 + texture(tex2, texCoord) * 0.2;
}