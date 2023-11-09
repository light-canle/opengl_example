#version 330 core

uniform vec4 color; // 프로그램으로부터 입력된 변수 (같은 변수명, 같은 타입)
out vec4 fragColor; // 최종 출력 색상

void main() {
    fragColor = color;
}
