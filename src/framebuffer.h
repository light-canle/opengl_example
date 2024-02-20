#ifndef __FRAMEBUFFER_H__
#define __FRAMEBUFFER_H__

#include "texture.h"

CLASS_PTR(Framebuffer);
class Framebuffer {
public:
    // 프레임 버퍼 생성
    static FramebufferUPtr Create(const std::vector<TexturePtr>& colorAttachments);
    // 기본 프레임 버퍼로 전환
    static void BindToDefault();
    ~Framebuffer();

    const uint32_t Get() const { return m_framebuffer; }
    // 바인딩 함수
    void Bind() const;
    // 프레임 버퍼에 attach된 color attachments의 개수를 반환
    int GetColorAttachmentCount() const { return (int)m_colorAttachments.size(); }
    // index에 해당하는 color attachment를 반환
    const TexturePtr GetColorAttachment(int index = 0) const { return m_colorAttachments[index]; }
private:
    Framebuffer() {}
    bool InitWithColorAttachments(const std::vector<TexturePtr>& colorAttachments);
    // 프레임 버퍼 ID
    uint32_t m_framebuffer { 0 };
    // 렌더 버퍼 ID
    uint32_t m_depthStencilBuffer { 0 };
    // 텍스쳐(컬러 버퍼)
    std::vector<TexturePtr> m_colorAttachments;
};

// 큐브맵을 color attachment로 하는 프레임버퍼
CLASS_PTR(CubeFramebuffer);
class CubeFramebuffer {
public:
  static CubeFramebufferUPtr Create(const CubeTexturePtr colorAttachment);
  ~CubeFramebuffer();

  const uint32_t Get() const { return m_framebuffer; }
  void Bind(int cubeIndex = 0) const;
  const CubeTexturePtr GetColorAttachment() const { return m_colorAttachment; }

private:
  CubeFramebuffer() {}
  bool InitWithColorAttachment(const CubeTexturePtr& colorAttachment);

  uint32_t m_framebuffer { 0 };
  uint32_t m_depthStencilBuffer { 0 };
  CubeTexturePtr m_colorAttachment;
};

#endif // __FRAMEBUFFER_H__