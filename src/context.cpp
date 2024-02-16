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
    // 박스 생성
    m_box = Mesh::MakeBox();
    // 평면 생성
    m_plane = Mesh::CreatePlane();

    // 프로그램 생성
    m_simpleProgram = Program::Create("./shader/simple.vert", "./shader/simple.frag");
    if (!m_simpleProgram)
        return false;

    m_program = Program::Create("./shader/lighting.vert", "./shader/lighting.frag");
    if (!m_program)
        return false;

    m_textureProgram = Program::Create("./shader/texture.vert", "./shader/texture.frag");
    if (!m_textureProgram)
      return false;

    m_postProgram = Program::Create("./shader/texture.vert", "./shader/gamma.frag");
    if (!m_postProgram)
        return false;

    // glfw 윈도우 배경색 지정(RGBA)
    glClearColor(0.1f, 0.2f, 0.3f, 1.0f);

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
    m_planeMaterial->shininess = 4.0f;
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

    // 스카이 박스
    auto cubeRight = Image::Load("./image/skybox/right.jpg", false); // pos x
    auto cubeLeft = Image::Load("./image/skybox/left.jpg", false); // neg x
    auto cubeTop = Image::Load("./image/skybox/top.jpg", false); // pos y
    auto cubeBottom = Image::Load("./image/skybox/bottom.jpg", false); // neg y
    auto cubeFront = Image::Load("./image/skybox/front.jpg", false); // pos z
    auto cubeBack = Image::Load("./image/skybox/back.jpg", false); // neg z
    m_cubeTexture = CubeTexture::CreateFromImages({
        cubeRight.get(),
        cubeLeft.get(),
        cubeTop.get(),
        cubeBottom.get(),
        cubeFront.get(),
        cubeBack.get(),
    });
    m_skyboxProgram = Program::Create("./shader/skybox.vert", "./shader/skybox.frag");
    m_envMapProgram = Program::Create("./shader/env_map.vert", "./shader/env_map.frag");

    // 쉐도우 맵
    m_shadowMap = ShadowMap::Create(1024, 1024);
    m_lightingShadowProgram = Program::Create("./shader/lighting_shadow.vert", "./shader/lighting_shadow.frag");

    // 노멀 맵
    m_brickDiffuseTexture = Texture::CreateFromImage(Image::Load("./image/brickwall.jpg", false).get());
    m_brickNormalTexture = Texture::CreateFromImage(Image::Load("./image/brickwall_normal.jpg", false).get());
    m_normalProgram = Program::Create("./shader/normal.vert", "./shader/normal.frag");

    // Deferred Shading - G-Buffer
    m_deferGeoProgram = Program::Create("./shader/defer_geo.vert", "./shader/defer_geo.frag");
    // Deferred Shading - Lighting pass
    m_deferLightProgram = Program::Create("./shader/defer_light.vert", "./shader/defer_light.frag");
    
    // 32개의 빛을 임의의 위치에 배치한 뒤, 색상을 부여
    m_deferLights.resize(32);
    for (size_t i = 0; i < m_deferLights.size(); i++) {
        m_deferLights[i].position = glm::vec3(
            RandomRange(-10.0f, 10.0f), RandomRange(1.0f, 4.0f), RandomRange(-10.0f, 10.0f));

        // SSAO맵의 결과를 명확하게 보기 위해 3개의 빛만 색을 부여한다.
        m_deferLights[i].color = glm::vec3(
            RandomRange(0.0f, i < 3 ? 1.0f : 0.0f),
            RandomRange(0.0f, i < 3 ? 1.0f : 0.0f),
            RandomRange(0.0f, i < 3 ? 1.0f : 0.0f));
    }

    // SSAO
    m_ssaoProgram = Program::Create("./shader/ssao.vert", "./shader/ssao.frag");
    m_blurProgram = Program::Create("./shader/blur_5x5.vert", "./shader/blur_5x5.frag");
    m_model = Model::Load("./model/backpack.obj");

    // random rotation vector 생성
    std::vector<glm::vec3> ssaoNoise;
    ssaoNoise.resize(16);
    for (size_t i = 0; i < ssaoNoise.size(); i++) {
        // randomly selected tangent direction
        // x, y만 난수인 이유는 z축에 가까운 normal vector에 대한 임의의 tangent vector를 만들어 내기 위해서이다.
        glm::vec3 sample(RandomRange(-1.0f, 1.0f), RandomRange(-1.0f, 1.0f), 0.0f);
        ssaoNoise[i] = sample;
    }

    // 4x4 크기의 텍스쳐 맵에 저장
    m_ssaoNoiseTexture = Texture::Create(4, 4, GL_RGB16F, GL_FLOAT);
    m_ssaoNoiseTexture->Bind();
    m_ssaoNoiseTexture->SetFilter(GL_NEAREST, GL_NEAREST);
    m_ssaoNoiseTexture->SetWrap(GL_REPEAT, GL_REPEAT);
    // 이미 바인딩된 텍스쳐의 데이터를 바꾸기 위해 glTexSubImage2D를 사용한다.
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 4, 4,
        GL_RGB, GL_FLOAT, ssaoNoise.data());

    // SSAO를 위한 커널 샘플의 위치 저장
    m_ssaoSamples.resize(64);
    for (size_t i = 0; i < m_ssaoSamples.size(); i++) {
        // uniformly randomized point in unit hemisphere
        glm::vec3 sample(
            RandomRange(-1.0f, 1.0f),
            RandomRange(-1.0f, 1.0f),
            RandomRange(0.0f, 1.0f));
        sample = glm::normalize(sample) * RandomRange();

        // scale for slightly shift to center
        // 더 좋은 차폐 연산을 위해서 모든 점들이 중심에 조금 더 가깝게 배치되도록 위치를 조절
        float t = (float)i / (float)m_ssaoSamples.size();
        float t2 = t * t;
        float scale = (1.0f - t2) * 0.1f + t2 * 1.0f;

        m_ssaoSamples[i] = sample * scale;
    }
    return true;
}

void Context::DrawScene(const glm::mat4& view, const glm::mat4& projection, const Program* program) {
    // 바닥과 3개의 상자의 transform을 각각 지정한 후 그린다.
    // 바닥
    program->Use();
    auto modelTransform =
        glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -0.5f, 0.0f)) *
        glm::scale(glm::mat4(1.0f), glm::vec3(40.0f, 1.0f, 40.0f));
    auto transform = projection * view * modelTransform;
    program->SetUniform("transform", transform);
    program->SetUniform("modelTransform", modelTransform);
    m_planeMaterial->SetToProgram(program);
    m_box->Draw(program);

    // 1번째 상자
    modelTransform =
        glm::translate(glm::mat4(1.0f), glm::vec3(-1.0f, 0.75f, -4.0f)) *
        glm::rotate(glm::mat4(1.0f), glm::radians(30.0f), glm::vec3(0.0f, 1.0f, 0.0f)) *
        glm::scale(glm::mat4(1.0f), glm::vec3(1.5f, 1.5f, 1.5f));
    transform = projection * view * modelTransform;
    program->SetUniform("transform", transform);
    program->SetUniform("modelTransform", modelTransform);
    m_box1Material->SetToProgram(program);
    m_box->Draw(program);

    // 2번째 상자
    modelTransform =
        glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.75f, 2.0f)) *
        glm::rotate(glm::mat4(1.0f), glm::radians(20.0f), glm::vec3(0.0f, 1.0f, 0.0f)) *
        glm::scale(glm::mat4(1.0f), glm::vec3(1.5f, 1.5f, 1.5f));
    transform = projection * view * modelTransform;
    program->SetUniform("transform", transform);
    program->SetUniform("modelTransform", modelTransform);
    m_box2Material->SetToProgram(program);
    m_box->Draw(program);

    // 3번째 상자 - 2번 상자와 동일한 텍스쳐를 사용
    modelTransform =
        glm::translate(glm::mat4(1.0f), glm::vec3(3.0f, 1.75f, -2.0f)) *
        glm::rotate(glm::mat4(1.0f), glm::radians(50.0f), glm::vec3(0.0f, 1.0f, 0.0f)) *
        glm::scale(glm::mat4(1.0f), glm::vec3(1.5f, 1.5f, 1.5f));
    transform = projection * view * modelTransform;
    program->SetUniform("transform", transform);
    program->SetUniform("modelTransform", modelTransform);
    m_box2Material->SetToProgram(program);
    m_box->Draw(program);

    // 가방 오브젝트 렌더링
    modelTransform =
        glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.55f, 0.0f)) *
        glm::rotate(glm::mat4(1.0f), glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f)) *
        glm::scale(glm::mat4(1.0f), glm::vec3(0.5f, 0.5f, 0.5f));
    transform = projection * view * modelTransform;
    program->SetUniform("transform", transform);
    program->SetUniform("modelTransform", modelTransform);
    m_model->Draw(program);
}

// 렌더링 담당 함수
void Context::Render(){
    // imgui
    if (ImGui::Begin("UI window")) {
        // 배경 색상
        if (ImGui::ColorEdit4("background color", glm::value_ptr(m_clearColor))) {
            glClearColor(m_clearColor.r, m_clearColor.g, m_clearColor.b, m_clearColor.a);
        }
        // 감마 값
        ImGui::DragFloat("gamma", &m_gamma, 0.01f, 0.0f, 2.0f);
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
            // Blinn-Phong Shading on/off
            ImGui::Checkbox("l.blinn", &m_blinn);
            // directional light on/off
            ImGui::Checkbox("l.directional", &m_light.directional);
            // SSAO setting
            ImGui::Checkbox("Use SSAO", &m_useSsao);
            ImGui::DragFloat("SSAO radius", &m_ssaoRadius, 0.01f, 0.01f, 5.0f);
        }
        
        // 큐브 회전 여부
        ImGui::Checkbox("animation", &m_animation);

        // 쉐도우 맵 시각화
        ImGui::Image((ImTextureID)m_shadowMap->GetShadowMap()->Get(),
            ImVec2(256, 256), ImVec2(0, 1), ImVec2(1, 0));
    }
    ImGui::End();

    // G-Buffer 시각화
    if (ImGui::Begin("G-Buffers")) {
        const char* bufferNames[] = { "position", "normal", "albedo/specular" };
        static int bufferSelect = 0;
        ImGui::Combo("buffer", &bufferSelect, bufferNames, 3);
        float width = ImGui::GetContentRegionAvailWidth();
        float height = width * ((float)m_height / (float)m_width);
        auto selectedAttachment =
        m_deferGeoFramebuffer->GetColorAttachment(bufferSelect);
        ImGui::Image((ImTextureID)selectedAttachment->Get(),
            ImVec2(width, height), ImVec2(0, 1), ImVec2(1, 0));
    }
    ImGui::End();

    // SSAO 시각화
    if (ImGui::Begin("SSAO")) {
        const char* bufferNames[] = { "original", "blurred" };
        static int bufferSelect = 0;
        ImGui::Combo("buffer", &bufferSelect, bufferNames, 2);

        float width = ImGui::GetContentRegionAvailWidth();
        float height = width * ((float)m_height / (float)m_width);
        auto selectedAttachment =
            bufferSelect == 0 ?
            m_ssaoFramebuffer->GetColorAttachment() :
            m_ssaoBlurFramebuffer->GetColorAttachment();

        ImGui::Image((ImTextureID)selectedAttachment->Get(),
        ImVec2(width, height), ImVec2(0, 1), ImVec2(1, 0));
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

    // 쉐도우 맵 렌더링
    // 빛의 시점에서 그림을 그리기 위해 light의 view, projection을 구함
    auto lightView = glm::lookAt(m_light.position,
        m_light.position + m_light.direction,
        glm::vec3(0.0f, 1.0f, 0.0f));
    // directional light는 projectional을 orthogonal로 하고, spot light는 perspective로 한다.
    auto lightProjection = m_light.directional ?
        glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, 1.0f, 30.0f) : 
        glm::perspective( glm::radians((m_light.cutoff[0] + m_light.cutoff[1]) * 2.0f), // fov 각도
            1.0f, 1.0f, 40.0f);

    // 프레임 버퍼 바인딩
    m_shadowMap->Bind();
    glClear(GL_DEPTH_BUFFER_BIT);
    // 프레임 버퍼 크기로 glViewport 초기화
    glViewport(0, 0,
        m_shadowMap->GetShadowMap()->GetWidth(),
        m_shadowMap->GetShadowMap()->GetHeight());
    // depth map을 계산할 쉐이더 프로그램
    m_simpleProgram->Use();
    m_simpleProgram->SetUniform("color", glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
    DrawScene(lightView, lightProjection, m_simpleProgram.get());

    // Deferred Shading을 이용한 G-Buffer 바인딩
    m_deferGeoFramebuffer->Bind();
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glViewport(0, 0, m_width, m_height);
    m_deferGeoProgram->Use();
    DrawScene(view, projection, m_deferGeoProgram.get());

    // SSAO
    m_ssaoFramebuffer->Bind();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glViewport(0, 0, m_width, m_height);
    // 프로그램 사용, 텍스쳐 바인딩
    m_ssaoProgram->Use();
    glActiveTexture(GL_TEXTURE0);
    m_deferGeoFramebuffer->GetColorAttachment(0)->Bind();
    glActiveTexture(GL_TEXTURE1);
    m_deferGeoFramebuffer->GetColorAttachment(1)->Bind();
    glActiveTexture(GL_TEXTURE2);
    m_ssaoNoiseTexture->Bind();
    glActiveTexture(GL_TEXTURE0);
    // Uniform 변수 지정
    m_ssaoProgram->SetUniform("gPosition", 0);
    m_ssaoProgram->SetUniform("gNormal", 1);
    m_ssaoProgram->SetUniform("texNoise", 2);
    m_ssaoProgram->SetUniform("noiseScale", glm::vec2(
        (float)m_width / (float)m_ssaoNoiseTexture->GetWidth(),
        (float)m_height / (float)m_ssaoNoiseTexture->GetHeight()));
    m_ssaoProgram->SetUniform("radius", m_ssaoRadius);
    for (size_t i = 0; i < m_ssaoSamples.size(); i++) {
        auto sampleName = fmt::format("samples[{}]", i);
        m_ssaoProgram->SetUniform(sampleName, m_ssaoSamples[i]);
    }
    m_ssaoProgram->SetUniform("transform", glm::scale(glm::mat4(1.0f), glm::vec3(2.0f)));
    m_ssaoProgram->SetUniform("view", view);
    m_ssaoProgram->SetUniform("projection", projection);
    m_plane->Draw(m_ssaoProgram.get());

    // SSAO blur
    m_ssaoBlurFramebuffer->Bind();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glViewport(0, 0, m_width, m_height);
    m_blurProgram->Use();
    m_ssaoFramebuffer->GetColorAttachment(0)->Bind();
    m_blurProgram->SetUniform("tex", 0);
    m_blurProgram->SetUniform("transform",
        glm::scale(glm::mat4(1.0f), glm::vec3(2.0f)));
    m_plane->Draw(m_blurProgram.get());

    Framebuffer::BindToDefault();
    glViewport(0, 0, m_width, m_height);
    glClearColor(m_clearColor.r, m_clearColor.g, m_clearColor.b, m_clearColor.a);

    // 생성한 프레임 버퍼 바인딩
    // 우리가 만든 프레임버퍼는 멀티 샘플이 아니므로 MSAA를 할 수 없다.
    // 그래서 이 부분은 주석 처리 했다.
    // m_framebuffer->Bind();

    // 윈도우 초기화 수행
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    // Z buffer 활성화
    glEnable(GL_DEPTH_TEST);

    // Deferred shading - Lighting pass
    m_deferLightProgram->Use();
    glActiveTexture(GL_TEXTURE0);
    m_deferGeoFramebuffer->GetColorAttachment(0)->Bind(); // position
    glActiveTexture(GL_TEXTURE1);
    m_deferGeoFramebuffer->GetColorAttachment(1)->Bind(); // normal
    glActiveTexture(GL_TEXTURE2);
    m_deferGeoFramebuffer->GetColorAttachment(2)->Bind(); // albedo, specualr
    glActiveTexture(GL_TEXTURE3);
    m_ssaoBlurFramebuffer->GetColorAttachment()->Bind(); // SSAO map
    glActiveTexture(GL_TEXTURE0);
    // 바인딩된 텍스쳐를 전달
    m_deferLightProgram->SetUniform("gPosition", 0);
    m_deferLightProgram->SetUniform("gNormal", 1);
    m_deferLightProgram->SetUniform("gAlbedoSpec", 2);
    m_deferLightProgram->SetUniform("ssao", 3);
    m_deferLightProgram->SetUniform("useSsao", m_useSsao ? 1 : 0);
    // 빛의 위치와 색상 전달
    for (size_t i = 0; i < m_deferLights.size(); i++) {
        auto posName = fmt::format("lights[{}].position", i);
        auto colorName = fmt::format("lights[{}].color", i);
        m_deferLightProgram->SetUniform(posName, m_deferLights[i].position);
        m_deferLightProgram->SetUniform(colorName, m_deferLights[i].color);
    }
    m_deferLightProgram->SetUniform("transform",
        glm::scale(glm::mat4(1.0f), glm::vec3(2.0f)));
    m_plane->Draw(m_deferLightProgram.get());
    
    // 올바른 렌더링을 위해 G-Buffer에 있는 depth buffer를 가져온다.
    glBindFramebuffer(GL_READ_FRAMEBUFFER, m_deferGeoFramebuffer->Get()); // 읽는 프레임 버퍼 지정
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0); // 추가로 그리는 프레임 버퍼
    glBlitFramebuffer(0, 0, m_width, m_height, // Read -> Draw쪽으로 버퍼를 복사 (여기서는 Depth Buffer만)
        0, 0, m_width, m_height,
        GL_DEPTH_BUFFER_BIT, GL_NEAREST);
    glBindFramebuffer(GL_FRAMEBUFFER, 0); // 기본 프레임 버퍼 바인딩

    // forward rendering
    m_simpleProgram->Use();
    for (size_t i = 0; i < m_deferLights.size(); i++) {
        m_simpleProgram->SetUniform("color", glm::vec4(m_deferLights[i].color, 1.0f));
        m_simpleProgram->SetUniform("transform", projection * view *
            glm::translate(glm::mat4(1.0f), m_deferLights[i].position) *
            glm::scale(glm::mat4(1.0f), glm::vec3(0.15f)));
        m_box->Draw(m_simpleProgram.get());
    }
    // Skybox 렌더링 - 크기 50짜리 큐브를 사용한다.
    // auto skyboxModelTransform =
    //   glm::translate(glm::mat4(1.0), m_cameraPos) *
    //   glm::scale(glm::mat4(1.0), glm::vec3(50.0f));
    // m_skyboxProgram->Use();
    // m_cubeTexture->Bind();
    // m_skyboxProgram->SetUniform("skybox", 0);
    // m_skyboxProgram->SetUniform("transform", projection * view * skyboxModelTransform);
    // m_box->Draw(m_skyboxProgram.get());

    // glm::vec3 lightPos = m_light.position;
    // glm::vec3 lightDir = m_light.direction;

    // 손전등 효과를 위한 추가 코드
    // spot light의 위치와 방향을 카메라와 일치시킨다.
    // if (m_flashLightMode) {
    //     m_light.position = m_cameraPos;
    //     m_light.direction = m_cameraFront;
    // }
    // else{
        // 조명 역할을 해주는 큐브 - flash light가 아닐 때만 그려짐
        // 빛의 위치
    //     auto lightModelTransform =
    //         glm::translate(glm::mat4(1.0), m_light.position) *
    //         glm::scale(glm::mat4(1.0f), glm::vec3(0.15f));
    //     m_simpleProgram->Use();
    //     m_simpleProgram->SetUniform("color", glm::vec4(m_light.ambient + m_light.diffuse, 1.0f));
    //     m_simpleProgram->SetUniform("transform", projection * view * lightModelTransform);
    //     m_box->Draw(m_simpleProgram.get());
    // }

    // /* === 물체 렌더링 === */
    // m_program->Use(); // 프로그램 사용
    // // 쉐이더 uniform 지정 (light 설정)
    // m_lightingShadowProgram->Use();
    // m_lightingShadowProgram->SetUniform("viewPos", m_cameraPos);
    // m_lightingShadowProgram->SetUniform("light.directional", m_light.directional ? 1 : 0);
    // m_lightingShadowProgram->SetUniform("light.position", m_light.position);
    // m_lightingShadowProgram->SetUniform("light.direction", m_light.direction);
    // m_lightingShadowProgram->SetUniform("light.cutoff", glm::vec2(
    //     cosf(glm::radians(m_light.cutoff[0])),
    //     cosf(glm::radians(m_light.cutoff[0] + m_light.cutoff[1]))));
    // m_lightingShadowProgram->SetUniform("light.attenuation", GetAttenuationCoeff(m_light.distance));
    // m_lightingShadowProgram->SetUniform("light.ambient", m_light.ambient);
    // m_lightingShadowProgram->SetUniform("light.diffuse", m_light.diffuse);
    // m_lightingShadowProgram->SetUniform("light.specular", m_light.specular);
    // m_lightingShadowProgram->SetUniform("blinn", (m_blinn ? 1 : 0));
    // // 빛의 projection, view 행렬을 전달
    // m_lightingShadowProgram->SetUniform("lightTransform", lightProjection * lightView);
    // // 쉐이더 맵은 3번 텍스쳐에 할당
    // glActiveTexture(GL_TEXTURE3);
    // m_shadowMap->GetShadowMap()->Bind();
    // // 쉐도우 맵의 ID를 전달
    // m_lightingShadowProgram->SetUniform("shadowMap", 3);
    // glActiveTexture(GL_TEXTURE0);

    // // 바닥과 3개의 상자의 transform을 각각 지정한 후 그린다.
    // DrawScene(view, projection, m_lightingShadowProgram.get());

    // auto modelTransform = 
    //     glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 3.0f, 0.0f)) *
    //     glm::rotate(glm::mat4(1.0f), glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    // m_normalProgram->Use();
    // m_normalProgram->SetUniform("viewPos", m_cameraPos);
    // m_normalProgram->SetUniform("lightPos", m_light.position);
    // glActiveTexture(GL_TEXTURE0);
    // m_brickDiffuseTexture->Bind();
    // m_normalProgram->SetUniform("diffuse", 0);
    // glActiveTexture(GL_TEXTURE1);
    // m_brickNormalTexture->Bind();
    // m_normalProgram->SetUniform("normalMap", 1);
    // glActiveTexture(GL_TEXTURE0);
    // m_normalProgram->SetUniform("modelTransform", modelTransform);
    // m_normalProgram->SetUniform("transform", projection * view * modelTransform);
    // m_plane->Draw(m_normalProgram.get());

    // 장면 그리기가 끝난 뒤 기본 프레임 버퍼로 바인딩을 바꾼다. 앞에서 렌더링한 장면이 텍스쳐로 저장되는데
    // 이 텍스쳐로 화면을 꽉 채우는 사각형을 그린다. - 결과는 이전과 다름이 없다.
    // 그러나 저 사각형의 transform을 조정함으로써 화면을 작거나 크게, 회전 시키는 등 다양한 변형이 가능하다.
    // 뿐만 아니라 렌더링된 화면이 텍스쳐이므로 다른 곳에 활용할 수도 있다.

    // 우리가 만든 프레임버퍼는 멀티 샘플이 아니므로 MSAA를 할 수 없다.
    // 그래서 이 부분은 주석 처리 했다.
    // Framebuffer::BindToDefault();

    // glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    // postProgram을 사용해서 렌더링된 화면에 포스트 프로세싱을 할 수 있다. (다른 fragment shader를 사용)
    // m_postProgram->Use();
    // m_postProgram->SetUniform("transform",
    //     glm::scale(glm::mat4(1.0f), glm::vec3(2.0f, 2.0f, 1.0f))); // 크기가 2인 이유는 화면상 좌표의 범위가 -1 ~ 1이기 때문
    // m_framebuffer->GetColorAttachment()->Bind();
    // m_postProgram->SetUniform("tex", 0);
    // m_postProgram->SetUniform("gamma", m_gamma);
    // m_plane->Draw(m_postProgram.get());
}

void Context::Reshape(int width, int height) {
    m_width = width;
    m_height = height;
    glViewport(0, 0, width, height);
    // 화면 크기와 동일한 프레임 버퍼 생성
    m_framebuffer = Framebuffer::Create({
        Texture::Create(width, height, GL_RGBA)
    });

    // G-Buffer 생성
    m_deferGeoFramebuffer = Framebuffer::Create({
        Texture::Create(width, height, GL_RGBA16F, GL_FLOAT), // position 저장
        Texture::Create(width, height, GL_RGBA16F, GL_FLOAT), // normal 저장
        Texture::Create(width, height, GL_RGBA, GL_UNSIGNED_BYTE), // albedo, specular 저장
    });

    // AO 값을 담는 단일 채널 프레임 버퍼 생성
    m_ssaoFramebuffer = Framebuffer::Create({
        Texture::Create(width, height, GL_RED),
    });
    // blur 효과 처리된 SSAO 맵
    m_ssaoBlurFramebuffer = Framebuffer::Create({
        Texture::Create(width, height, GL_RED),
    });
}