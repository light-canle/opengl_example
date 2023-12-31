#ifndef __TEXTURE_H__
#define __TEXTURE_H__

#include "image.h"

CLASS_PTR(Texture)
class Texture {
public:
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
    
private:
    // 생성자
    Texture() {}
    void CreateTexture();
    void SetTextureFromImage(const Image* image);
    // 텍스쳐 ID
    uint32_t m_texture { 0 };
};

#endif // __TEXTURE_H__