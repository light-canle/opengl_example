#ifndef __SHADER_H__
#define __SHADER_H__

#include "common.h"

CLASS_PTR(Shader);
class Shader{
public:
    // 이 함수를 통해서 shader를 생성할 수 있다.
    static ShaderUPtr CreateFromFile(const std::string& filename, GLenum shaderType);

    ~Shader();
    // Get만 있음, set은 쉐이더 파일 내부에서 직접 한다.
    uint32_t Get() const { return m_shader; }

private:
    Shader() {} // 쉐이더 생성자를 private로 설정
    // 파일 로드 - 실패하면 false 반환
    bool LoadFile(const std::string& filename, GLenum shaderType);
    uint32_t m_shader {0};
};

#endif