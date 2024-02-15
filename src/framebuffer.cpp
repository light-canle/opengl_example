#include "framebuffer.h"

FramebufferUPtr Framebuffer::Create(const std::vector<TexturePtr>& colorAttachments) {
    // 프레임 버퍼 생성
    auto framebuffer = FramebufferUPtr(new Framebuffer());
    if (!framebuffer->InitWithColorAttachments(colorAttachments))
        return nullptr;
    return std::move(framebuffer);
}

Framebuffer::~Framebuffer() {
    if (m_depthStencilBuffer) {
        glDeleteRenderbuffers(1, &m_depthStencilBuffer);
    }
    if (m_framebuffer) {
        glDeleteFramebuffers(1, &m_framebuffer);
    }
}

void Framebuffer::BindToDefault() {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Framebuffer::Bind() const {
    glBindFramebuffer(GL_FRAMEBUFFER, m_framebuffer);
}

bool Framebuffer::InitWithColorAttachments(const std::vector<TexturePtr>& colorAttachments) {
    // 컬러 텍스쳐를 가져옴
    m_colorAttachments = colorAttachments;
    // 프레임 버퍼 오브젝트 생성(1개), 바인딩
    glGenFramebuffers(1, &m_framebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, m_framebuffer);

    // 프레임 버퍼에 컬러 버퍼(텍스쳐)를 붙임
    for (size_t i = 0; i < m_colorAttachments.size(); i++) {
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, 
            GL_TEXTURE_2D, m_colorAttachments[i]->Get(), 0);
    }

    // 컬러 버퍼의 번호(GL_COLOR_ATTACHMENT0 ~)들을 저장하는 vector 생성(glDrawBuffers에 전달하기 위함)
    if (m_colorAttachments.size() > 0) {
        std::vector<GLenum> attachments;
        attachments.resize(m_colorAttachments.size());
        for (size_t i = 0; i < m_colorAttachments.size(); i++)
            attachments[i] = GL_COLOR_ATTACHMENT0 + i;
        glDrawBuffers(m_colorAttachments.size(), attachments.data());
    }

    int width = m_colorAttachments[0]->GetWidth();
    int height = m_colorAttachments[0]->GetHeight();

    // depth, stencil 버퍼를 위한 렌더 버퍼 생성, 바인딩
    glGenRenderbuffers(1, &m_depthStencilBuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, m_depthStencilBuffer);
    // 렌더 버퍼의 타입 지정
    glRenderbufferStorage(
        GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, // depth, stencil 버퍼의 타입(하나로 붙임)
        width, height); // 크기 지정
    // 기본 프레임 버퍼로 다시 되돌림
    glBindRenderbuffer(GL_RENDERBUFFER, 0);

    // 프레임 버퍼에 렌더 버퍼를 붙임
    glFramebufferRenderbuffer(
        GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT,
        GL_RENDERBUFFER, m_depthStencilBuffer);
    // 해당 렌더 버퍼의 조합을 프레임 버퍼로 사용할 수 있는지 확인
    auto result = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if (result != GL_FRAMEBUFFER_COMPLETE) {
        SPDLOG_ERROR("failed to create framebuffer: {}", result);
        return false;
    }

    BindToDefault();

    return true;
}