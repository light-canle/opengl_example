#ifndef __CONTEXT_H__
#define __CONTEXT_H__

#include "common.h"
#include "shader.h"
#include "program.h"
#include "buffer.h"
#include "vertex_layout.h"
#include "texture.h"

CLASS_PTR(Context)
CLASS_PTR(Buffer)
CLASS_PTR(VertexLayout)
class Context {
public:
    // 생성 함수
    static ContextUPtr Create();
    // 렌더링 함수
    void Render();
    // 키 입력 처리 함수
    void ProcessInput(GLFWwindow* window);
    // 새로운 화면 크기로 창 재설정
    void Reshape(int width, int height);
    // 마우스 이동에 대한 카메라 방향 처리
    void MouseMove(double x, double y);
    // 마우스 클릭 처리 함수
    void MouseButton(int button, int action, double x, double y);
private:
    Context() {}
    // 초기화 함수
    bool Init();
    // 프로그램 저장용 포인터
    ProgramUPtr m_program;
    ProgramUPtr m_simpleProgram;

    // VAO 오브젝트
    VertexLayoutUPtr m_vertexLayout;
    // VBO 오브젝트
    BufferUPtr m_vertexBuffer;
    // EBO 오브젝트
    BufferUPtr m_indexBuffer;
    // Texture 오브젝트
    TextureUPtr m_texture;
    TextureUPtr m_texture2;

    // camera parameter
    glm::vec3 m_cameraPos { glm::vec3(0.0f, 0.0f, 3.0f) }; // p = e 카메라 위치 벡터
    glm::vec3 m_cameraFront { glm::vec3(0.0f, 0.0f, -1.0f) }; // 카메라가 보고 있는 방향
    glm::vec3 m_cameraUp { glm::vec3(0.0f, 1.0f, 0.0f) }; // u 벡터(up 벡터)
    float m_cameraPitch { 0.0f }; // x축 기준 회전값(위/아래)
    float m_cameraYaw { 0.0f }; // y축 기준 회전값(좌/우)
    bool m_cameraControl { false }; // 카메라를 움직일 지 여부(마우스 오른쪽 키를 누르는 동안 true)
    glm::vec2 m_prevMousePos { glm::vec2(0.0f) }; // 카메라 이전 위치

    // 현재 화면 크기
    int m_width { WINDOW_WIDTH };
    int m_height { WINDOW_HEIGHT };

    // clear color(배경 색상)
    glm::vec4 m_clearColor { glm::vec4(0.1f, 0.2f, 0.3f, 0.0f) };

    // cube animation (큐브의 회전 여부)
    bool m_animation { true };

    // light (조명)
    struct Light {
        glm::vec3 position { glm::vec3(2.0f, 2.0f, 2.0f) };
        glm::vec3 direction { glm::vec3(-1.0f, -1.0f, -1.0f) };
        // 단위는 육십분법, 1번째는 inner, 2번째는 outer와 inner의 차이 각도가 들어간다.
        glm::vec2 cutoff { glm::vec2(20.0f, 5.0f) };
        float distance { 32.0f };
        glm::vec3 ambient { glm::vec3(0.1f, 0.1f, 0.1f) };
        glm::vec3 diffuse { glm::vec3(0.5f, 0.5f, 0.5f) };
        glm::vec3 specular { glm::vec3(1.0f, 1.0f, 1.0f) };
    };
    Light m_light;

    // Material (재질)
    struct Material {
        TextureUPtr diffuse;
        TextureUPtr specular;
        float shininess { 32.0f };
    };
    Material m_material;
};

#endif