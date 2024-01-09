#include "context.h"
#include "image.h"

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
    // 정점 데이터
    float vertices[] = {
        // x, y, z(정점 위치), s, t(정점의 텍스쳐 좌표)
        -0.5f, -0.5f, -0.5f, 0.0f, 0.0f,
         0.5f, -0.5f, -0.5f, 1.0f, 0.0f,
         0.5f,  0.5f, -0.5f, 1.0f, 1.0f,
        -0.5f,  0.5f, -0.5f, 0.0f, 1.0f,

        -0.5f, -0.5f,  0.5f, 0.0f, 0.0f,
         0.5f, -0.5f,  0.5f, 1.0f, 0.0f,
         0.5f,  0.5f,  0.5f, 1.0f, 1.0f,
        -0.5f,  0.5f,  0.5f, 0.0f, 1.0f,

        -0.5f,  0.5f,  0.5f, 1.0f, 0.0f,
        -0.5f,  0.5f, -0.5f, 1.0f, 1.0f,
        -0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f, 0.0f, 0.0f,

        0.5f,  0.5f,  0.5f, 1.0f, 0.0f,
        0.5f,  0.5f, -0.5f, 1.0f, 1.0f,
        0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
        0.5f, -0.5f,  0.5f, 0.0f, 0.0f,

        -0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
         0.5f, -0.5f, -0.5f, 1.0f, 1.0f,
         0.5f, -0.5f,  0.5f, 1.0f, 0.0f,
        -0.5f, -0.5f,  0.5f, 0.0f, 0.0f,

        -0.5f,  0.5f, -0.5f, 0.0f, 1.0f,
         0.5f,  0.5f, -0.5f, 1.0f, 1.0f,
         0.5f,  0.5f,  0.5f, 1.0f, 0.0f,
        -0.5f,  0.5f,  0.5f, 0.0f, 0.0f,
    };

    // 인덱스 데이터
    uint32_t indices[] = {
         0,  2,  1,  2,  0,  3,
         4,  5,  6,  6,  7,  4,
         8,  9, 10, 10, 11,  8,
        12, 14, 13, 14, 12, 15,
        16, 17, 18, 18, 19, 16,
        20, 22, 21, 22, 20, 23,
    };

    // 1. VAO 바인딩 - VBO 바인딩 전에 함(그래야 VAO에 대응하는 VBO가 지정된다.)
    m_vertexLayout = VertexLayout::Create();

    // 2. VBO 바인딩 / 정점 데이터 복사
    // 마지막 인자에 곱해지는 값은 정점 리스트의 데이터 크기이다.
    m_vertexBuffer = Buffer::CreateWithData(GL_ARRAY_BUFFER, GL_STATIC_DRAW, vertices, sizeof(float) * 120);

    // 3. VAO에서 정점 데이터의 속성을 지정
    // 5번째 인자에 곱해지는 값은 한 정점 당 데이터 크기이다.(지금은 위치 3개, 텍스쳐 2개로 총 5개)
    m_vertexLayout->SetAttrib(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 5, 0); // 위치(0-2)
    m_vertexLayout->SetAttrib(2, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 5, sizeof(float) * 3); // 텍스쳐 좌표(3-4)

    // 4. EBO 바인딩
    // 4번째 인자에 곱해지는 수는 인덱스 배열의 길이이다.
    m_indexBuffer = Buffer::CreateWithData(GL_ELEMENT_ARRAY_BUFFER, GL_STATIC_DRAW, indices, sizeof(uint32_t) * 36);

    // 쉐이더 초기화
    ShaderPtr vertShader = Shader::CreateFromFile("./shader/texture.vert", GL_VERTEX_SHADER);
    ShaderPtr fragShader = Shader::CreateFromFile("./shader/texture.frag", GL_FRAGMENT_SHADER);
    // 쉐이더 초기화 실패한 경우
    if (!vertShader || !fragShader)
        return false;
    // 쉐이더 ID 출력
    SPDLOG_INFO("vertex shader id: {}", vertShader->Get());
    SPDLOG_INFO("fragment shader id: {}", fragShader->Get());

    // 프로그램 생성/정보 출력
    m_program = Program::Create({fragShader, vertShader});
    if (!m_program)
        return false;
    SPDLOG_INFO("program id: {}", m_program->Get());

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
    
    // 두 텍스쳐를 각각 텍스쳐 슬롯 0, 1번에 넣는다.
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_texture->Get());
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, m_texture2->Get());

    // 프래그먼트 쉐이더에 바인딩을 할 두 텍스쳐의 텍스쳐 슬롯 번호를 Uniform으로 전달해준다.
    m_program->Use();
    m_program->SetUniform("tex", 0);
    m_program->SetUniform("tex2", 1);

    return true;
}

// Note : 확대 -> 회전 -> 평행이동 순으로 점에 선형 변환 적용
// translate로 평행이동, rotate로 회전, scale로 크기 변경

// Uniform 변수 값 지정 - 프로그램 생성 이후에 배치
// 프로그램에서 color uniform 변수의 위치를 가져옴
// auto loc = glGetUniformLocation(m_program->Get(), "color");
// 해당 위치에 값을 넣음(color는 vec4이다.)
// glUniform4f(loc, 1.0f, 1.0f, 0.0f, 1.0f);

/*
// lookAt 함수는 아래와 같이 카메라 행렬의 역행렬을 계산해준다.
auto cameraZ = glm::normalize(cameraPos - cameraTarget); // 카메라 z방향은 e - t 벡터의 방향이다.
auto cameraX = glm::normalize(glm::cross(cameraUp, cameraZ)); // 카메라 x 방향은 up 벡터와 z방향의 외적이다.
auto cameraY = glm::cross(cameraZ, cameraX); // 카메라 y방향은 z방향과 x방향의 외적이다.

// 구한 벡터들로 카메라 행렬을 구함
auto cameraMat = glm::mat4(
    glm::vec4(cameraX, 0.0f),
    glm::vec4(cameraY, 0.0f),
    glm::vec4(cameraZ, 0.0f),
    glm::vec4(cameraPos, 1.0f));

auto view = glm::inverse(cameraMat); // view 행렬은 카메라 행렬의 역행렬이다.
*/

// 렌더링 담당 함수
void Context::Render(){
    // 서로 다른 큐브들의 위치 리스트
    std::vector<glm::vec3> cubePositions = {
        glm::vec3( 0.0f, 0.0f, 0.0f),
        glm::vec3( 2.0f, 5.0f, -15.0f),
        glm::vec3(-1.5f, -2.2f, -2.5f),
        glm::vec3(-3.8f, -2.0f, -12.3f),
        glm::vec3( 2.4f, -0.4f, -3.5f),
        glm::vec3(-1.7f, 3.0f, -7.5f),
        glm::vec3( 1.3f, -2.0f, -2.5f),
        glm::vec3( 1.5f, 2.0f, -2.5f),
        glm::vec3( 1.5f, 0.2f, -1.5f),
        glm::vec3(-1.3f, 1.0f, -1.5f),
    };

    // 윈도우 초기화 수행
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    // Z buffer 활성화
    glEnable(GL_DEPTH_TEST);
    
    // 프로그램 사용
    m_program->Use();

    // 카메라의 front(바라보는 방향)를 계산
    // (0, 0, -1) 벡터(w = 0)를 yaw, pitch 값에 따라 회전
    m_cameraFront = glm::rotate(glm::mat4(1.0f), glm::radians(m_cameraYaw), glm::vec3(0.0f, 1.0f, 0.0f)) * 
                    glm::rotate(glm::mat4(1.0f), glm::radians(m_cameraPitch), glm::vec3(1.0f, 0.0f, 0.0f)) * 
                    glm::vec4(0.0f, 0.0f, -1.0f, 0.0f);

    // 큐브 회전을 위한 새로운 MVP 행렬
    // 화각(FOV), 종횡비(aspect ratio), near~far 설정
    auto projection = glm::perspective(glm::radians(45.0f),
      (float)m_width / (float)m_height, 0.01f, 100.0f);

    // 카메라의 위치, 타겟 위치를 이용한 view 행렬 계산
    auto view = glm::lookAt(m_cameraPos, m_cameraFront + m_cameraPos, m_cameraUp);

    // 큐브마다 서로 위치, 회전을 다르게 한다.
    for (size_t i = 0; i < cubePositions.size(); i++){
        auto& pos = cubePositions[i];
        // 정해진 위치로 이동
        auto model = glm::translate(glm::mat4(1.0f), pos);
        model = glm::rotate(model,
            glm::radians((float)glfwGetTime() * 120.0f + 20.0f * (float)i), // glfwGetTime()을 이용해서 회전 각을 변경해준다.
            glm::vec3(1.0f, 0.5f, 0.0f));
        auto transform = projection * view * model;
        m_program->SetUniform("transform", transform);
        // 화면에 그리는 코드
        // 그리는 방식, 정점 개수, 인덱스 데이터 타입, 오프셋을 지정
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
    }
}

void Context::Reshape(int width, int height) {
    m_width = width;
    m_height = height;
    glViewport(0, 0, width, height);
}