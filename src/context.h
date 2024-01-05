#ifndef __CONTEXT_H__
#define __CONTEXT_H__

#include "common.h"
#include "shader.h"
#include "program.h"
#include "buffer.h"
#include "vertex_layout.h"
#include "texture.h"

CLASS_PTR(Context)
CLASS_PTR(Buffer)
CLASS_PTR(VertexLayout)
class Context {
public:
    // 생성 함수
    static ContextUPtr Create();
    // 렌더링 함수
    void Render();    
private:
    Context() {}
    // 초기화 함수
    bool Init();
    // 프로그램 저장용 포인터
    ProgramUPtr m_program;

    // VAO 오브젝트
    VertexLayoutUPtr m_vertexLayout;
    // VBO 오브젝트
    BufferUPtr m_vertexBuffer;
    // EBO 오브젝트
    BufferUPtr m_indexBuffer;
    // Texture 오브젝트
    TextureUPtr m_texture;
    TextureUPtr m_texture2;
};

#endif