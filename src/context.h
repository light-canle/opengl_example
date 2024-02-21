#ifndef __CONTEXT_H__
#define __CONTEXT_H__

#include "common.h"
#include "shader.h"
#include "program.h"
#include "buffer.h"
#include "vertex_layout.h"
#include "texture.h"
#include "mesh.h"
#include "model.h"
#include "framebuffer.h"
#include "shadow_map.h"

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
    // 쉐이더 프로그램을 이용해서 그림을 그리는 함수
    // 카메라의 view, projection matrix와 프로그램을 인자로 받음
    void DrawScene(const glm::mat4& view, const glm::mat4& projection, const Program* program);
private:
    Context() {}
    // 초기화 함수
    bool Init();
    // 프로그램 저장용 포인터
    ProgramUPtr m_simpleProgram;
    ProgramUPtr m_pbrProgram;
    ProgramUPtr m_sphericalMapProgram;
    ProgramUPtr m_skyboxProgram;
    ProgramUPtr m_diffuseIrradianceProgram; // Diffuse Irradiance map
    ProgramUPtr m_preFilteredProgram; // Pre-Filtered map
    ProgramUPtr m_brdfLookupProgram; // BRDF lookup table

    // Mesh
    MeshUPtr m_box;
    MeshUPtr m_plane;
    MeshUPtr m_sphere;

    // Texture
    TextureUPtr m_hdrMap; // IBL용 hdr 맵
    TexturePtr m_brdfLookupMap; // IBL 용 BRDF lookup table

    // CubeMap
    CubeTexturePtr m_hdrCubeMap;
    CubeTexturePtr m_diffuseIrradianceMap; // Diffuse Irradiance map
    CubeTexturePtr m_preFilteredMap; // Pre-Filtered map

    // camera parameter
    glm::vec3 m_cameraPos { glm::vec3(0.0f, 2.5f, 8.0f) }; // p = e 카메라 위치 벡터
    glm::vec3 m_cameraFront { glm::vec3(0.0f, 0.0f, -1.0f) }; // 카메라가 보고 있는 방향
    glm::vec3 m_cameraUp { glm::vec3(0.0f, 1.0f, 0.0f) }; // u 벡터(up 벡터)
    float m_cameraPitch { -20.0f }; // x축 기준 회전값(위/아래)
    float m_cameraYaw { 0.0f }; // y축 기준 회전값(좌/우)
    bool m_cameraControl { false }; // 카메라를 움직일 지 여부(마우스 오른쪽 키를 누르는 동안 true)
    glm::vec2 m_prevMousePos { glm::vec2(0.0f) }; // 카메라 이전 위치

    // 현재 화면 크기
    int m_width { WINDOW_WIDTH };
    int m_height { WINDOW_HEIGHT };

    // 빛
    struct Light {
        glm::vec3 position { glm::vec3(0.0f, 0.0f, 0.0f) };
        glm::vec3 color { glm::vec3(1.0f, 1.0f, 1.0f) };
    };
    std::vector<Light> m_lights;
    bool m_useIBL { true }; // IBL 사용 여부

    // 머터리얼(재질)
    struct Material {
        glm::vec3 albedo { glm::vec3(1.0f, 1.0f, 1.0f) };
        float roughness { 0.5f };
        float metallic { 0.5f };
        float ao { 0.1f };
    };
    Material m_material;
};

#endif