#include "context.h"

ContextUPtr Context::Create() {
    auto context = ContextUPtr(new Context());
    if (!context->Init())
        return nullptr;
    return std::move(context);
}

bool Context::Init() {
    // 정점 데이터
    float vertices[] = {
        0.5f, 0.5f, 0.0f, // top right(0)
        0.5f, -0.5f, 0.0f, // bottom right(1)
        -0.5f, -0.5f, 0.0f, // bottom left(2)
        -0.5f, 0.5f, 0.0f, // top left(3)
    };
    // 인덱스 데이터
    uint32_t indices[] = { // note that we start from 0!
        0, 1, 3, // first triangle
        1, 2, 3, // second triangle
    };

    // 1. VAO 바인딩 - VBO 바인딩 전에 함(그래야 VAO에 대응하는 VBO가 지정된다.)
    m_vertexLayout = VertexLayout::Create();

    // 2. VBO 바인딩 / 정점 데이터 복사
    m_vertexBuffer = Buffer::CreateWithData(GL_ARRAY_BUFFER, GL_STATIC_DRAW, vertices, sizeof(float) * 12);

    // 3. VAO에서 정점 데이터의 속성을 지정
    m_vertexLayout->SetAttrib(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, 0);

    // 4. EBO 바인딩
    m_indexBuffer = Buffer::CreateWithData(GL_ELEMENT_ARRAY_BUFFER, GL_STATIC_DRAW, indices, sizeof(uint32_t) * 6);

    // 쉐이더 초기화
    ShaderPtr vertShader = Shader::CreateFromFile("./shader/simple.vert", GL_VERTEX_SHADER);
    ShaderPtr fragShader = Shader::CreateFromFile("./shader/simple.frag", GL_FRAGMENT_SHADER);
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

    // glfw 윈도우 배경색 지정(RGBA)
    glClearColor(0.1f, 0.2f, 0.3f, 1.0f);

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