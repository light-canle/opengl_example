#include "texture.h"

TextureUPtr Texture::CreateFromImage(const Image* image) {
    // 텍스쳐 인스턴스 생성
    auto texture = TextureUPtr(new Texture());
    // 텍스쳐 생성
    texture->CreateTexture();
    texture->SetTextureFromImage(image);
    return std::move(texture);
}

Texture::~Texture() {
    if (m_texture) {  
        // 메모리 해제
        glDeleteTextures(1, &m_texture);
    }
}

void Texture::Bind() const {
    // 텍스쳐 바인딩
    glBindTexture(GL_TEXTURE_2D, m_texture);
}

void Texture::SetFilter(uint32_t minFilter, uint32_t magFilter) const {
    // 텍스쳐 필터 (MIN은 작을 때, MAG는 클 때) - 텍스쳐를 이루는 픽셀을 화면에 어떻게 표시할 것인가?
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minFilter);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, magFilter);
}

void Texture::SetWrap(uint32_t sWrap, uint32_t tWrap) const {
    // 텍스쳐 래핑(0~1을 벗어난 좌표에 대한 처리)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, sWrap);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, tWrap);
}
    
void Texture::CreateTexture() {
    // 텍스쳐 생성
    glGenTextures(1, &m_texture);
    // bind and set default filter and wrap option (바인딩된 텍스쳐의 속성 설정)
    Bind();
    // 필터와 wrap 옵션의 기본값을 넣음
    SetFilter(GL_LINEAR, GL_LINEAR);
    SetWrap(GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);
}

void Texture::SetTextureFromImage(const Image* image) {
    GLenum format = GL_RGBA;
    // 채널 수에 따른 색상 포맷 결정
    switch (image->GetChannelCount()) {
        default: break;
        case 1: format = GL_RED; break;
        case 2: format = GL_RG; break;
        case 3: format = GL_RGB; break;
    }
    // 이미지 데이터를 GPU에 복사
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA,
        image->GetWidth(), image->GetHeight(), 0, // 이미지 가로, 세로, 보더 크기(이미지 주위를 감싸는 테두리)
        format, GL_UNSIGNED_BYTE, image->GetData()); // 이미지 데이터 타입, 1채널 데이터 타입, 실제 이미지 데이터
}