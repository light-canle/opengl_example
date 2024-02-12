#include "texture.h"

// 일반 텍스쳐는 type에 GL_UNSIGNED_BYTE를 쓰고, 쉐도우 맵은 GL_FLOAT를 쓴다.
TextureUPtr Texture::Create(int width, int height, uint32_t format, uint32_t type) {
    auto texture = TextureUPtr(new Texture());
    texture->CreateTexture();
    texture->SetTextureFormat(width, height, format, type);
    texture->SetFilter(GL_LINEAR, GL_LINEAR);
    return std::move(texture);
}

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
    SetFilter(GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
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

    // 크기와 포맷, 픽셀 타입 설정
    m_width = image->GetWidth();
    m_height = image->GetHeight();
    m_format = format;
    m_type = GL_UNSIGNED_BYTE;

    // 이미지 데이터를 GPU에 복사
    glTexImage2D(GL_TEXTURE_2D, 0, m_format,
        m_width, m_height, 0, // 이미지 가로, 세로, 보더 크기(이미지 주위를 감싸는 테두리)
        format, m_type, image->GetData()); // 이미지 데이터 타입, 1채널 데이터 타입, 실제 이미지 데이터

    // 텍스쳐를 위한 mipmap(이미지에 대한 더 작은 사본)을 만들어냄
    glGenerateMipmap(GL_TEXTURE_2D);
}

void Texture::SetTextureFormat(int width, int height, uint32_t format, uint32_t type) {
    m_width = width;
    m_height = height;
    m_format = format;
    m_type = type;

    glTexImage2D(GL_TEXTURE_2D, 0, m_format,
        m_width, m_height, 0,
        m_format, type, nullptr);
}

/* == 큐브맵 텍스쳐 함수 == */

CubeTextureUPtr CubeTexture::CreateFromImages(const std::vector<Image*> &images) {
    auto texture = CubeTextureUPtr(new CubeTexture());
    if (!texture->InitFromImages(images))
        return nullptr;
    return std::move(texture);
}
// 일반 텍스쳐 소멸자와 비슷함
CubeTexture::~CubeTexture() {
    if (m_texture) {
        glDeleteTextures(1, &m_texture);
    }
}

void CubeTexture::Bind() const {
    // 바인딩 - 타입이 GL_TEXTURE_CUBE_MAP이다.
    glBindTexture(GL_TEXTURE_CUBE_MAP, m_texture);    
}

bool CubeTexture::InitFromImages(const std::vector<Image*> &images) {
    glGenTextures(1, &m_texture);
    Bind();

    // 큐브맵 텍스쳐 초기화
    // 일반 텍스쳐와 다르게 타입을 GL_TEXTURE_CUBE_MAP로 지정하고
    // 래핑은 WRAP_S, WRAP_T, WRAP_R 3차원에 대해 해준다.
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    // 6장의 이미지를 가져와 포맷을 설정해준뒤, 이미지를 텍스쳐 메모리에 복사해준다.
    for (uint32_t i = 0; i < (uint32_t)images.size(); i++) {
        auto image = images[i];
        GLenum format = GL_RGBA;
        switch (image->GetChannelCount()) {
            default: break;
            case 1: format = GL_RED; break;
            case 2: format = GL_RG; break;
            case 3: format = GL_RGB; break;
        }

        // 6개에 대해 이미지를 모두 복사해주어야 함 - GL_TEXTURE_CUBE_MAP_POSITIVE_X부터 순차적으로 6개에 지정
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB,
            image->GetWidth(), image->GetHeight(), 0,
            format, GL_UNSIGNED_BYTE, image->GetData());
    }

    return true;
}

void Texture::SetBorderColor(const glm::vec4& color) const {
    // 보더의 색깔 지정
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, glm::value_ptr(color));
}