#include "program.h"

ProgramUPtr Program::Create(const std::vector<ShaderPtr>& shaders) {
    auto program = ProgramUPtr(new Program());
    if (!program->Link(shaders))
        return nullptr;
    return std::move(program);
}

ProgramUPtr Program::Create(const std::string& vertShaderFilename, const std::string& fragShaderFilename) {
    // 쉐이더 초기화
    ShaderPtr vs = Shader::CreateFromFile(vertShaderFilename, GL_VERTEX_SHADER);
    ShaderPtr fs = Shader::CreateFromFile(fragShaderFilename, GL_FRAGMENT_SHADER);
    // 쉐이더 초기화 실패한 경우
    if (!vs || !fs)
        return nullptr;
    // 성공한 경우 두 쉐이더를 링크한 프로그램을 반환
    return std::move(Create({vs, fs}));
}

bool Program::Link(const std::vector<ShaderPtr>& shaders) {
    // 프로그램 생성
    m_program = glCreateProgram();
    // 쉐이더들을 프로그램에 붙임
    for (auto& shader: shaders)
        glAttachShader(m_program, shader->Get());
    // 프로그램 링크
    glLinkProgram(m_program);

    // 프로그램 링크가 성공적으로 이루어졌는지 판단
    int success = 0;
    glGetProgramiv(m_program, GL_LINK_STATUS, &success);
    // 실패한 경우 오류 출력
    if (!success) {
        char infoLog[1024];
        glGetProgramInfoLog(m_program, 1024, nullptr, infoLog);
        SPDLOG_ERROR("failed to link program: {}", infoLog);
        return false;
    }
    return true;
}

// 프로그램 소멸자
Program::~Program() {
    if (m_program) {
        glDeleteProgram(m_program);
    }
}

// 프로그램 사용
void Program::Use() const{
    glUseProgram(m_program);
}

// Uniform 변수의 값을 바꾼다.
void Program::SetUniform(const std::string& name, int value) const {
    auto loc = glGetUniformLocation(m_program, name.c_str());
    glUniform1i(loc, value);
}

void Program::SetUniform(const std::string& name, const glm::mat4& value) const {
    auto loc = glGetUniformLocation(m_program, name.c_str());
    glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(value)); // value_ptr을 이용해 행렬을 전달
}

void Program::SetUniform(const std::string& name, float value) const {
    auto loc = glGetUniformLocation(m_program, name.c_str());
    glUniform1f(loc, value);
}

void Program::SetUniform(const std::string& name, const glm::vec3& value) const {
    auto loc = glGetUniformLocation(m_program, name.c_str());
    glUniform3fv(loc, 1, glm::value_ptr(value));
}

void Program::SetUniform(const std::string& name, const glm::vec4& value) const {
    auto loc = glGetUniformLocation(m_program, name.c_str());
    glUniform4fv(loc, 1, glm::value_ptr(value));
}