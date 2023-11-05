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
        -0.5f, -0.5f, 0.0f,
         0.5f, -0.5f, 0.0f,
         0.0f,  0.5f, 0.0f,
    };

    // 1. VAO 바인딩 - VBO 바인딩 전에 함(그래야 VAO에 대응하는 VBO가 지정된다.)
    glGenVertexArrays(1, &m_vertexArrayObject);
    glBindVertexArray(m_vertexArrayObject);

    // 2. VBO 바인딩 / 정점 데이터 복사
    glGenBuffers(1, &m_vertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, m_vertexBuffer); // 버퍼 바인딩(지금 사용할 버퍼 오브젝트를 결정)
    // 정점 데이터 복사
    // GL_STATIC_DRAW는 이 버퍼에 데이터를 1번 넣은 뒤 바꾸지 않겠다는 것을 뜻함
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 9, vertices, GL_STATIC_DRAW); 

    // 3. VAO에서 정점 데이터의 속성을 지정
    glEnableVertexAttribArray(0); // 0번 속성(Position)
    // 0번 속성의 데이터의 정보(데이터 수, 한 데이터의 타입, 정규화 여부, 바이트 크기, 오프셋) 지정
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, 0);

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
    glUseProgram(m_program->Get());

    // 화면에 그리는 코드
    // 그리는 방식, 시작 인덱스, 정점 개수를 지정
    glDrawArrays(GL_TRIANGLES, 0, 3);
}