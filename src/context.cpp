#include "context.h"
#include "image.h"

ContextUPtr Context::Create() {
    auto context = ContextUPtr(new Context());
    if (!context->Init())
        return nullptr;
    return std::move(context);
}

bool Context::Init() {
    // 정점 데이터
    float vertices[] = {
        // x, y, z(정점 위치), r, g, b(정점 색깔), s, t(정점의 텍스쳐 좌표)
         0.5f,  0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, // 
         0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,
        -0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
        -0.5f,  0.5f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f,
    };
    // 인덱스 데이터
    uint32_t indices[] = { // note that we start from 0!
        0, 1, 3, // first triangle
        1, 2, 3, // second triangle
    };

    // 1. VAO 바인딩 - VBO 바인딩 전에 함(그래야 VAO에 대응하는 VBO가 지정된다.)
    m_vertexLayout = VertexLayout::Create();

    // 2. VBO 바인딩 / 정점 데이터 복사
    // 마지막 인자에 곱해지는 값은 정점 리스트의 데이터 크기이다.
    m_vertexBuffer = Buffer::CreateWithData(GL_ARRAY_BUFFER, GL_STATIC_DRAW, vertices, sizeof(float) * 32);

    // 3. VAO에서 정점 데이터의 속성을 지정
    // 5번째 인자에 곱해지는 값은 한 정점 당 데이터 크기이다.(지금은 위치 3개, 색깔 3개, 텍스쳐 2개로 총 8개)
    m_vertexLayout->SetAttrib(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 8, 0); // 위치(0-2)
    m_vertexLayout->SetAttrib(1, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 8, sizeof(float) * 3); // 색상(3-5)
    m_vertexLayout->SetAttrib(2, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 8, sizeof(float) * 6); // 텍스쳐 좌표(6-7)

    // 4. EBO 바인딩
    m_indexBuffer = Buffer::CreateWithData(GL_ELEMENT_ARRAY_BUFFER, GL_STATIC_DRAW, indices, sizeof(uint32_t) * 6);

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

    // Uniform 변수 값 지정 - 프로그램 생성 이후에 배치
    // 프로그램에서 color uniform 변수의 위치를 가져옴
    // auto loc = glGetUniformLocation(m_program->Get(), "color");
    m_program->Use(); // Use 실행
    // 해당 위치에 값을 넣음(color는 vec4이다.)
    // glUniform4f(loc, 1.0f, 1.0f, 0.0f, 1.0f);

    // glfw 윈도우 배경색 지정(RGBA)
    glClearColor(0.1f, 0.2f, 0.3f, 1.0f);

    // 텍스쳐에 쓸 이미지 로드
    // auto image = Image::Load("./image/container.jpg");
    // if (!image) 
    //     return false;
    // 이미지 정보 출력
    // SPDLOG_INFO("image: {}x{}, {} channels", 
    //     image->GetWidth(), image->GetHeight(), image->GetChannelCount());

    // 체커 보드 이미지 생성
    auto image = Image::Create(512, 512);
    image->SetCheckImage(16, 16);

    // 텍스쳐 생성
    m_texture = Texture::CreateFromImage(image.get());

    return true;
}

// 렌더링 담당 함수
void Context::Render(){
    // 윈도우 초기화 수행
    glClear(GL_COLOR_BUFFER_BIT);
    
    // 프로그램 사용
    m_program->Use();
    // 화면에 그리는 코드
    // 그리는 방식, 정점 개수, 인덱스 데이터 타입, 오프셋을 지정
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}