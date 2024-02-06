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
    const float cameraSpeed = 0.01f;
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
    // 박스 생성
    m_box = Mesh::MakeBox();

    // 프로그램 생성/정보 출력
    m_simpleProgram = Program::Create("./shader/simple.vert", "./shader/simple.frag");
    if (!m_simpleProgram)
        return false;

    m_program = Program::Create("./shader/lighting.vert", "./shader/lighting.frag");
    if (!m_program)
        return false;

    m_program->Use(); // Use 실행

    // glfw 윈도우 배경색 지정(RGBA)
    glClearColor(0.1f, 0.2f, 0.3f, 1.0f);

    // 텍스쳐에 쓸 이미지 로드
    auto image = Image::Load("./image/container.jpg");
    if (!image) 
        return false;
    // 이미지 정보 출력
    SPDLOG_INFO("image: {}x{}, {} channels", 
        image->GetWidth(), image->GetHeight(), image->GetChannelCount());

    // 텍스쳐 생성
    m_texture = Texture::CreateFromImage(image.get());

    auto image2 = Image::Load("./image/awesomeface.png");
    if (!image2) 
        return false;
    m_texture2 = Texture::CreateFromImage(image2.get());

    // specular 맵으로 사용할 텍스쳐
    // 1번 상자의 specular 텍스쳐
    TexturePtr darkGrayTexture = Texture::CreateFromImage(
        Image::CreateSingleColorImage(4, 4, glm::vec4(0.2f, 0.2f, 0.2f, 1.0f)).get());
    // 바닥의 specular 텍스쳐
    TexturePtr grayTexture = Texture::CreateFromImage(
        Image::CreateSingleColorImage(4, 4, glm::vec4(0.5f, 0.5f, 0.5f, 1.0f)).get());
    
    // 바닥과 두 상자의 텍스쳐 맵을 불러온다.
    // 바닥
    m_planeMaterial = Material::Create();
    m_planeMaterial->diffuse = Texture::CreateFromImage(Image::Load("./image/marble.jpg").get());
    m_planeMaterial->specular = grayTexture;
    m_planeMaterial->shininess = 128.0f;
    // 1번 상자
    m_box1Material = Material::Create();
    m_box1Material->diffuse = Texture::CreateFromImage(Image::Load("./image/container.jpg").get());
    m_box1Material->specular = darkGrayTexture;
    m_box1Material->shininess = 16.0f;
    // 2번 상자
    m_box2Material = Material::Create();
    m_box2Material->diffuse = Texture::CreateFromImage(Image::Load("./image/container2.png").get());
    m_box2Material->specular = Texture::CreateFromImage(Image::Load("./image/container2_specular.png").get());
    m_box2Material->shininess = 64.0f;

    return true;
}

// 렌더링 담당 함수
void Context::Render(){
    // imgui
    if (ImGui::Begin("UI window")) {
        // 배경 색상
        if (ImGui::ColorEdit4("background color", glm::value_ptr(m_clearColor))) {
            glClearColor(m_clearColor.r, m_clearColor.g, m_clearColor.b, m_clearColor.a);
        }
        // 메뉴 구분선
        ImGui::Separator();
        // 카메라 위치, 회전을 바꾸기 위한 drag
        ImGui::DragFloat3("camera pos", glm::value_ptr(m_cameraPos), 0.01f);
        ImGui::DragFloat("camera yaw", &m_cameraYaw, 0.5f, -360.0f, 360.0f);
        ImGui::DragFloat("camera pitch", &m_cameraPitch, 0.5f, -89.0f, 89.0f);
        ImGui::Separator();
        // 카메라 위치 초기화 버튼
        if (ImGui::Button("reset camera")) {
            m_cameraYaw = 0.0f;
            m_cameraPitch = 0.0f;
            m_cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
        }
        // 배경 색상 초기화 버튼
        if (ImGui::Button("reset background")) {
            m_clearColor = glm::vec4(0.1f, 0.2f, 0.3f, 0.0f);
            glClearColor(m_clearColor.r, m_clearColor.g, m_clearColor.b, m_clearColor.a);
        }
        // 조명
        if (ImGui::CollapsingHeader("light", ImGuiTreeNodeFlags_DefaultOpen)) {
            ImGui::DragFloat3("l.position", glm::value_ptr(m_light.position), 0.01f);
            ImGui::DragFloat3("l.direction", glm::value_ptr(m_light.direction), 0.01f);
            ImGui::DragFloat2("l.cutoff", glm::value_ptr(m_light.cutoff), 0.1f, 0.0f, 180.0f);
            ImGui::DragFloat("l.distance", &m_light.distance, 0.1f, 0.1f, 3000.0f);
            ImGui::ColorEdit3("l.ambient", glm::value_ptr(m_light.ambient));
            ImGui::ColorEdit3("l.diffuse", glm::value_ptr(m_light.diffuse));
            ImGui::ColorEdit3("l.specular", glm::value_ptr(m_light.specular));
            ImGui::Checkbox("flash light", &m_flashLightMode);
        }
        // 큐브 회전 여부
        ImGui::Checkbox("animation", &m_animation);
    }
    ImGui::End();

    // 윈도우 초기화 수행
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    // Z buffer 활성화
    glEnable(GL_DEPTH_TEST);

    // 카메라의 front(바라보는 방향)를 계산
    // (0, 0, -1) 벡터(w = 0)를 yaw, pitch 값에 따라 회전
    m_cameraFront = glm::rotate(glm::mat4(1.0f), glm::radians(m_cameraYaw), glm::vec3(0.0f, 1.0f, 0.0f)) * 
                    glm::rotate(glm::mat4(1.0f), glm::radians(m_cameraPitch), glm::vec3(1.0f, 0.0f, 0.0f)) * 
                    glm::vec4(0.0f, 0.0f, -1.0f, 0.0f);

    // 큐브 회전을 위한 새로운 MVP 행렬
    // 화각(FOV), 종횡비(aspect ratio), near~far 설정
    auto projection = glm::perspective(glm::radians(45.0f),
      (float)m_width / (float)m_height, 0.1f, 100.0f);

    // 카메라의 위치, 타겟 위치를 이용한 view 행렬 계산
    auto view = glm::lookAt(m_cameraPos, m_cameraFront + m_cameraPos, m_cameraUp);

    /* view matrix를 구했으므로 이제 큐브들을 배치한다. */
    
    glm::vec3 lightPos = m_light.position;
    glm::vec3 lightDir = m_light.direction;

    // 손전등 효과를 위한 추가 코드
    // spot light의 위치와 방향을 카메라와 일치시킨다.
    if (m_flashLightMode) {
        m_light.position = m_cameraPos;
        m_light.direction = m_cameraFront;
    }
    else{
        // 조명 역할을 해주는 큐브 - flash light가 아닐 때만 그려짐
        // 빛의 위치
        auto lightModelTransform =
            glm::translate(glm::mat4(1.0), m_light.position) *
            glm::scale(glm::mat4(1.0f), glm::vec3(0.15f));
        m_simpleProgram->Use();
        m_simpleProgram->SetUniform("color", glm::vec4(m_light.ambient + m_light.diffuse, 1.0f));
        m_simpleProgram->SetUniform("transform", projection * view * lightModelTransform);
        m_box->Draw(m_simpleProgram.get());
    }

    /* === 물체 렌더링 === */
    m_program->Use(); // 프로그램 사용
    // 쉐이더 uniform 지정 (light 설정)
    m_program->SetUniform("viewPos", m_cameraPos);
    m_program->SetUniform("light.position", lightPos);
    m_program->SetUniform("light.direction", lightDir);
    m_program->SetUniform("light.cutoff", glm::vec2(
        cosf(glm::radians(m_light.cutoff[0])),
        cosf(glm::radians(m_light.cutoff[0] + m_light.cutoff[1]))));
    m_program->SetUniform("light.attenuation", GetAttenuationCoeff(m_light.distance));
    m_program->SetUniform("light.ambient", m_light.ambient);
    m_program->SetUniform("light.diffuse", m_light.diffuse);
    m_program->SetUniform("light.specular", m_light.specular);

    // 바닥과 2개의 상자의 transform을 각각 지정한 후 그린다.
    // 바닥
    auto modelTransform = 
        glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -0.5f, 0.0f)) * 
        glm::scale(glm::mat4(1.0f), glm::vec3(10.0f, 1.0f, 10.0f));
    auto transform = projection * view * modelTransform;
    m_program->SetUniform("transform", transform);
    m_program->SetUniform("modelTransform", modelTransform);
    m_planeMaterial->SetToProgram(m_program.get());
    m_box->Draw(m_program.get());

    // 1번째 상자
    modelTransform =
        glm::translate(glm::mat4(1.0f), glm::vec3(-1.0f, 0.75f, -4.0f)) *
        glm::rotate(glm::mat4(1.0f), glm::radians(30.0f), glm::vec3(0.0f, 1.0f, 0.0f)) *
        glm::scale(glm::mat4(1.0f), glm::vec3(1.5f, 1.5f, 1.5f));
    transform = projection * view * modelTransform;
    m_program->SetUniform("transform", transform);
    m_program->SetUniform("modelTransform", modelTransform);
    m_box1Material->SetToProgram(m_program.get());
    m_box->Draw(m_program.get());

    // 2번째 상자에는 스텐실 버퍼를 이용해 외각선을 그릴 것이다.
    // 스텐실 테스트 활성화
    glEnable(GL_STENCIL_TEST);
    // depth test에 실패하면(상자가 다른 물체에 가려지면) 스텐실 값이 그대로이고, 
    // 성공하면 glStencilFunc의 2번째 인자인 1로 바뀐다.
    glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
    glStencilFunc(GL_ALWAYS, 1, 0xFF); // GL_ALWAYS이므로 스텐실 테스트는 언제나 성공한다.
    glStencilMask(0xFF);

    // 2번째 상자를 그린다.
    modelTransform =
        glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.75f, 2.0f)) *
        glm::rotate(glm::mat4(1.0f), glm::radians(20.0f), glm::vec3(0.0f, 1.0f, 0.0f)) *
        glm::scale(glm::mat4(1.0f), glm::vec3(1.5f, 1.5f, 1.5f));
    transform = projection * view * modelTransform;
    m_program->SetUniform("transform", transform);
    m_program->SetUniform("modelTransform", modelTransform);
    m_box2Material->SetToProgram(m_program.get());
    m_box->Draw(m_program.get());

    // 2번째 상자에 외각선을 그리기 위한 스텐실 함수 설정
    // 스텐실 값이 1이 아닌 픽셀에만 그림을 그린다.
    glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
    glStencilMask(0x00);
    glDisable(GL_DEPTH_TEST); // 외각선은 위치 상관없이 그려지도록 하기 위함이다.
    // 외각선을 위한 shader 설정
    m_simpleProgram->Use();
    m_simpleProgram->SetUniform("color", glm::vec4(1.0f, 1.0f, 0.5f, 1.0f));
    // 원래 상자보다 1.05배 크게 그린다. 이때 이미 그려진 상자를 이루는 픽셀의 스텐실 값은 1이므로
    // 위의 설정에 의해 기존의 상자 부분은 그대로 유지한 채 외각선 부분만 그리게 된다.
    m_simpleProgram->SetUniform("transform", transform *
    glm::scale(glm::mat4(1.0f), glm::vec3(1.05f, 1.05f, 1.05f)));
    m_box->Draw(m_simpleProgram.get());

    // 원래대로 되돌림
    glEnable(GL_DEPTH_TEST);
    glDisable(GL_STENCIL_TEST);
    glStencilFunc(GL_ALWAYS, 1, 0xFF);
    glStencilMask(0xFF);
}

void Context::Reshape(int width, int height) {
    m_width = width;
    m_height = height;
    glViewport(0, 0, width, height);
}