#include "vertex_layout.h"

VertexLayoutUPtr VertexLayout::Create() {
    auto vertexLayout = VertexLayoutUPtr(new VertexLayout());
    vertexLayout->Init();
    return std::move(vertexLayout);
}

// VAO 삭제
VertexLayout::~VertexLayout() {
    if (m_vertexArrayObject) {
        glDeleteVertexArrays(1, &m_vertexArrayObject);
    }
}

void VertexLayout::Bind() const {
    // VAO 바인딩
    glBindVertexArray(m_vertexArrayObject);
}

void VertexLayout::SetAttrib(
    uint32_t attribIndex, int count,
    uint32_t type, bool normalized,
    size_t stride, uint64_t offset) const 
{
    glEnableVertexAttribArray(attribIndex);
    // attribIndex번 속성의 데이터의 정보(데이터 수, 한 데이터의 타입, 정규화 여부, 바이트 크기, 오프셋) 지정
    glVertexAttribPointer(attribIndex, count,
        type, normalized, stride, (const void*)offset);
}

// 초기화 함수 - Init는 반드시 성공하므로 return이 없다.
void VertexLayout::Init() {
    // VAO 생성
    glGenVertexArrays(1, &m_vertexArrayObject);
    Bind();
}