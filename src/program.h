#ifndef __PROGRAM_H__
#define __PROGRAM_H__

#include "common.h"
#include "shader.h"

CLASS_PTR(Program)
class Program {
public:
    // 외부에서 쓰는 생성자
    // 여러 개의 쉐이더들을 받을 수 있다 - 쉐이더는 다른 곳에서 쓸 수도 있으므로 shared_ptr로 받는다.
    static ProgramUPtr Create(const std::vector<ShaderPtr>& shaders);

    ~Program();
    uint32_t Get() const { return m_program; }    
private:
    Program() {}
    // 초기화 함수
    bool Link(const std::vector<ShaderPtr>& shaders);
    // Program ID
    uint32_t m_program { 0 };
};

#endif