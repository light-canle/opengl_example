#ifndef __TEXTURE_H__
#define __TEXTURE_H__

#include "image.h"

CLASS_PTR(Texture)
class Texture {
public:
    // 크기가 정해진 빈 텍스쳐를 만듦
    static TextureUPtr Create(int width, int height, uint32_t format, uint32_t type = GL_UNSIGNED_BYTE);
    // 이미지로부터 텍스쳐를 만들어냄(소유권 상관없이 포인터를 받기 위해 일반 포인터 사용)
    static TextureUPtr CreateFromImage(const Image* image);
    ~Texture();

    const uint32_t Get() const { return m_texture; }
    // 텍스쳐 바인딩
    void Bind() const;
    // Min, Mag 필터 설정
    void SetFilter(uint32_t minFilter, uint32_t magFilter) const;
    // Wrap 모드 설정
    void SetWrap(uint32_t sWrap, uint32_t tWrap) const;
    // Wrapping이 GL_CLAMP_TO_BORDER일 때, border의 색을 지정
    void SetBorderColor(const glm::vec4& color) const;

    // 텍스쳐의 크기와 포맷, 픽셀 타입을 반환
    int GetWidth() const { return m_width; }
    int GetHeight() const { return m_height; }
    uint32_t GetFormat() const { return m_format; }
    uint32_t GetType() const { return m_type; }

private:
    // 생성자
    Texture() {}
    void CreateTexture();
    void SetTextureFromImage(const Image* image);
    void SetTextureFormat(int width, int height, uint32_t format, uint32_t type);
    // 텍스쳐 ID
    uint32_t m_texture { 0 };
    // 크기와 포맷, 픽셀 타입
    int m_width { 0 };
    int m_height { 0 };
    uint32_t m_format { GL_RGBA };
    uint32_t m_type { GL_UNSIGNED_BYTE };
};

// 큐브맵을 위한 텍스쳐
CLASS_PTR(CubeTexture)
class CubeTexture {
public:
    // 이미지들(6장)로 부터 큐브맵 텍스쳐 생성
    static CubeTextureUPtr CreateFromImages(const std::vector<Image*> &images);
    // 특정 타입의 빈 텍스쳐 맵 생성
    static CubeTextureUPtr Create(int width, int height, uint32_t format, uint32_t type = GL_UNSIGNED_BYTE);
    ~CubeTexture();

    const uint32_t Get() const { return m_texture; }
    void Bind() const;

    int GetWidth() const { return m_width; }
    int GetHeight() const { return m_height; }
    uint32_t GetFormat() const { return m_format; }
    uint32_t GetType() const { return m_type; }
private:
    CubeTexture() {}
    bool InitFromImages(const std::vector<Image*> &images);
    void Init(int width, int height, int format, uint32_t type);
    // 텍스쳐 ID
    uint32_t m_texture { 0 };
    int m_width { 0 };
    int m_height { 0 };
    uint32_t m_format { GL_RGBA };
    uint32_t m_type { GL_UNSIGNED_BYTE };
};

#endif // __TEXTURE_H__