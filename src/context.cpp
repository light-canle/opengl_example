#include "context.h"
#include "image.h"
#include <imgui.h>

ContextUPtr Context::Create() {
    auto context = ContextUPtr(new Context());
    if (!context->Init())
        return nullptr;
    return std::move(context);
}

// 입력 처리(카메라 이동)
void Context::ProcessInput(GLFWwindow* window) {
    // 마우스 오른쪽 클릭을 유지한 상태에서만 이동
    if (!m_cameraControl)
        return;
    // 카메라 앞/뒤 이동
    const float cameraSpeed = 0.05f;
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        m_cameraPos += cameraSpeed * m_cameraFront;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        m_cameraPos -= cameraSpeed * m_cameraFront;

    // 카메라 좌우 이동
    // 카메라 오른쪽 방향은 up 벡터와 -front 벡터의 외적이다.
    auto cameraRight = glm::normalize(glm::cross(m_cameraUp, -m_cameraFront)); 
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        m_cameraPos += cameraSpeed * cameraRight;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        m_cameraPos -= cameraSpeed * cameraRight;    

    // 카메라 상하 이동
    auto cameraUp = glm::normalize(glm::cross(-m_cameraFront, cameraRight));
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
        m_cameraPos += cameraSpeed * cameraUp;
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
        m_cameraPos -= cameraSpeed * cameraUp;
}

// 마우스 입력(카메라 방향)
void Context::MouseMove(double x, double y) {
    // 마우스 오른쪽 클릭을 유지한 상태에서만 이동
    if (!m_cameraControl)
        return;
    // 현재 위치 구함
    auto pos = glm::vec2((float)x, (float)y);
    auto deltaPos = pos - m_prevMousePos;

    const float cameraRotSpeed = 0.8f;
    // 카메라 좌우 각
    m_cameraYaw -= deltaPos.x * cameraRotSpeed;
    // 카메라 상하 값
    m_cameraPitch -= deltaPos.y * cameraRotSpeed;

    // 카메라 각도의 상한과 하한을 설정
    if (m_cameraYaw < 0.0f)   m_cameraYaw += 360.0f;
    if (m_cameraYaw > 360.0f) m_cameraYaw -= 360.0f;

    if (m_cameraPitch > 89.0f)  m_cameraPitch = 89.0f;
    if (m_cameraPitch < -89.0f) m_cameraPitch = -89.0f;

    // 이전 위치 저장
    m_prevMousePos = pos;
}

// 마우스 키 입력
void Context::MouseButton(int button, int action, double x, double y) {
    if (button == GLFW_MOUSE_BUTTON_RIGHT) {
        if (action == GLFW_PRESS) {
            // 마우스 조작 시작 시점에 현재 마우스 커서 위치 저장
            m_prevMousePos = glm::vec2((float)x, (float)y);
            m_cameraControl = true;
        }
        else if (action == GLFW_RELEASE) {
            m_cameraControl = false;
        }
    }
}

bool Context::Init() {
    // MSAA 활성화
    glEnable(GL_MULTISAMPLE);
    // Z buffer 활성화
    glEnable(GL_DEPTH_TEST);
    // glfw 윈도우 배경색 지정(RGBA)
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    // 박스 생성
    m_box = Mesh::MakeBox();
    // 평면 생성
    m_plane = Mesh::CreatePlane();
    // 구 생성
    m_sphere = Mesh::CreateSphere();

    // 프로그램 생성
    m_simpleProgram = Program::Create("./shader/simple.vert", "./shader/simple.frag");
    if (!m_simpleProgram)
        return false;
    m_pbrProgram = Program::Create("./shader/pbr_texture.vert", "./shader/pbr_texture.frag");
    if (!m_pbrProgram)
        return false;

    // 이미지(텍스쳐 맵) 불러오기
    m_material.albedo = Texture::CreateFromImage(
        Image::Load("./image/rustediron2_basecolor.png").get());
    m_material.roughness = Texture::CreateFromImage(
        Image::Load("./image/rustediron2_roughness.png").get());
    m_material.metallic = Texture::CreateFromImage(
        Image::Load("./image/rustediron2_metallic.png").get());
    m_material.normal = Texture::CreateFromImage(
        Image::Load("./image/rustediron2_normal.png").get());
    
    // 빛 생성
    m_lights.push_back({ glm::vec3(5.0f, 5.0f, 6.0f), glm::vec3(40.0f, 40.0f, 40.0f) });
    m_lights.push_back({ glm::vec3(-4.0f, 5.0f, 7.0f), glm::vec3(40.0f, 40.0f, 40.0f) });
    m_lights.push_back({ glm::vec3(-4.0f, -6.0f, 8.0f), glm::vec3(40.0f, 40.0f, 40.0f) });
    m_lights.push_back({ glm::vec3(5.0f, -6.0f, 9.0f), glm::vec3(40.0f, 40.0f, 40.0f) });

    return true;
}

void Context::DrawScene(const glm::mat4& view, const glm::mat4& projection, const Program* program) {
    program->Use();
 
    const int sphereCount = 7;
    const float offset = 1.2f;
    // 7x7 배치로 49개의 구를 그린다.
    for (int j = 0; j < sphereCount; j++) {
        float y = ((float)j - (float)(sphereCount - 1) * 0.5f) * offset;
        for (int i = 0; i < sphereCount; i++) {
            float x = ((float)i - (float)(sphereCount - 1) * 0.5f) * offset;
            auto modelTransform =
                glm::translate(glm::mat4(1.0f), glm::vec3(x, y, 0.0f));
            auto transform = projection * view * modelTransform;
            program->SetUniform("transform", transform);
            program->SetUniform("modelTransform", modelTransform);
            m_sphere->Draw(program);
        }
    }
}

// 렌더링 담당 함수
void Context::Render(){
    // imgui
    if (ImGui::Begin("UI window")) {
        // 카메라 위치, 회전을 바꾸기 위한 drag
        ImGui::DragFloat3("camera pos", glm::value_ptr(m_cameraPos), 0.01f);
        ImGui::DragFloat("camera yaw", &m_cameraYaw, 0.5f, -360.0f, 360.0f);
        ImGui::DragFloat("camera pitch", &m_cameraPitch, 0.5f, -89.0f, 89.0f);
        // 메뉴 구분선
        ImGui::Separator();
        // 카메라 위치 초기화 버튼
        if (ImGui::Button("reset camera")) {
            m_cameraYaw = 0.0f;
            m_cameraPitch = 0.0f;
            m_cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
        }

        if (ImGui::CollapsingHeader("lights")) {
            static int lightIndex = 0;
            ImGui::DragInt("light.index", &lightIndex, 1.0f, 0, (int)m_lights.size() - 1);
            ImGui::DragFloat3("light.pos", glm::value_ptr(m_lights[lightIndex].position), 0.01f);
            ImGui::DragFloat3("light.color", glm::value_ptr(m_lights[lightIndex].color), 0.1f);
        }
        if (ImGui::CollapsingHeader("material")) {
            // ImGui::ColorEdit3("mat.albedo", glm::value_ptr(m_material.albedo));
            // ImGui::SliderFloat("mat.roughness", &m_material.roughness, 0.0f, 1.0f);
            // ImGui::SliderFloat("mat.metallic", &m_material.metallic, 0.0f, 1.0f);
            ImGui::SliderFloat("mat.ao", &m_material.ao, 0.0f, 1.0f);
        }
    }
    ImGui::End();

    // 카메라의 front(바라보는 방향)를 계산
    // (0, 0, -1) 벡터(w = 0)를 yaw, pitch 값에 따라 회전
    m_cameraFront = glm::rotate(glm::mat4(1.0f), glm::radians(m_cameraYaw), glm::vec3(0.0f, 1.0f, 0.0f)) * 
                    glm::rotate(glm::mat4(1.0f), glm::radians(m_cameraPitch), glm::vec3(1.0f, 0.0f, 0.0f)) * 
                    glm::vec4(0.0f, 0.0f, -1.0f, 0.0f);

    // 큐브 회전을 위한 새로운 MVP 행렬
    // 화각(FOV), 종횡비(aspect ratio), near~far 설정
    auto projection = glm::perspective(glm::radians(45.0f),
      (float)m_width / (float)m_height, 0.01f, 150.0f);

    // 카메라의 위치, 타겟 위치를 이용한 view 행렬 계산
    auto view = glm::lookAt(m_cameraPos, m_cameraFront + m_cameraPos, m_cameraUp);

    // 윈도우 초기화 수행
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    // forward rendering - 이 아래부터 그림을 그림
    m_pbrProgram->Use();
    m_pbrProgram->SetUniform("viewPos", m_cameraPos);
    m_pbrProgram->SetUniform("material.ao", m_material.ao);
    m_pbrProgram->SetUniform("material.albedo", 0);
    m_pbrProgram->SetUniform("material.roughness", 1);
    m_pbrProgram->SetUniform("material.metallic", 2);
    m_pbrProgram->SetUniform("material.normal", 3);
    glActiveTexture(GL_TEXTURE0);
    m_material.albedo->Bind();
    glActiveTexture(GL_TEXTURE1);
    m_material.roughness->Bind();
    glActiveTexture(GL_TEXTURE2);
    m_material.metallic->Bind();
    glActiveTexture(GL_TEXTURE3);
    m_material.normal->Bind();
    glActiveTexture(GL_TEXTURE0);
    for (size_t i = 0; i < m_lights.size(); i++) {
        auto posName = fmt::format("lights[{}].position", i);
        auto colorName = fmt::format("lights[{}].color", i);
        m_pbrProgram->SetUniform(posName, m_lights[i].position);
        m_pbrProgram->SetUniform(colorName, m_lights[i].color);
    }
    DrawScene(view, projection, m_pbrProgram.get());
}

void Context::Reshape(int width, int height) {
    m_width = width;
    m_height = height;
    glViewport(0, 0, width, height);
}