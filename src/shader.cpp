#include "shader.h"

ShaderUPtr Shader::CreateFromFile(const std::string& filename, GLenum shaderType) {
    // 쉐이더 생성
    auto shader = std::unique_ptr<Shader>(new Shader());
    // 파일을 가져오고 잘 가져왔는지 검사
    if (!shader->LoadFile(filename, shaderType))
        return nullptr; // 가져오기 실패
    // 가져오기 성공 - shader가 소유한 데이터를 반환한다.
    return std::move(shader);
}

bool Shader::LoadFile(const std::string& filename, GLenum shaderType) {
    auto result = LoadTextFile(filename);
    // 값이 있는지 확인(소스가 로딩 되었는지 확인)
    if (!result.has_value())
        return false;

    // 코드를 가져와 레퍼런스에 저장(크기가 클 수 있기 때문)
    auto& code = result.value();
    // const char*로 변환
    const char* codePtr = code.c_str();
    // 코드 길이
    int32_t codeLength = (int32_t)code.length();

    // create and compile shader
    // 쉐이더를 만들고 ID 저장
    m_shader = glCreateShader(shaderType);
    // 소스 파일 가져옴 - (ID, 개수, 코드(const char*), 길이)
    glShaderSource(m_shader, 1, (const GLchar* const*)&codePtr, &codeLength);
    glCompileShader(m_shader);

    // check compile error
    int success = 0;
    // 쉐이더의 정보(여기서는 컴파일 성공 여부)를 얻어옴
    glGetShaderiv(m_shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        // 컴파일에 실패한 경우 에러 메시지 출력
        char infoLog[1024];
        glGetShaderInfoLog(m_shader, 1024, nullptr, infoLog);
        SPDLOG_ERROR("failed to compile shader: \"{}\"", filename);
        SPDLOG_ERROR("reason: {}", infoLog);
        return false;
    }
    return true;
}

Shader::~Shader() {
    // 쉐이더 제거
    if (m_shader) {
        glDeleteShader(m_shader);
    }
}