#ifndef __FRAMEBUFFER_H__
#define __FRAMEBUFFER_H__

#include "texture.h"

CLASS_PTR(Framebuffer);
class Framebuffer {
public:
    // 프레임 버퍼 생성
    static FramebufferUPtr Create(const TexturePtr colorAttachment);
    // 기본 프레임 버퍼로 전환
    static void BindToDefault();
    ~Framebuffer();

    const uint32_t Get() const { return m_framebuffer; }
    // 바인딩 함수
    void Bind() const;
    // 프레임 버퍼 내 텍스쳐 반환
    const TexturePtr GetColorAttachment() const { return m_colorAttachment; }

private:
    Framebuffer() {}
    bool InitWithColorAttachment(const TexturePtr colorAttachment);
    // 프레임 버퍼 ID
    uint32_t m_framebuffer { 0 };
    // 렌더 버퍼 ID
    uint32_t m_depthStencilBuffer { 0 };
    // 텍스쳐(컬러 버퍼)
    TexturePtr m_colorAttachment;
};

#endif // __FRAMEBUFFER_H__