#include "image.h"
#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

ImageUPtr Image::Load(const std::string& filepath) {
    // 이미지 객체 만듦
    auto image = ImageUPtr(new Image());
    // 초기화
    if (!image->LoadWithStb(filepath))
        return nullptr;
    return std::move(image);
}

Image::~Image() {
    if (m_data) {
        // 이미지 데이터 삭제
        stbi_image_free(m_data);
    }
}

// 파일 경로를 받아 stb 라이브러리를 사용해서 이미지 생성
bool Image::LoadWithStb(const std::string& filepath) {
    m_data = stbi_load(filepath.c_str(), &m_width, &m_height, &m_channelCount, 0);
    if (!m_data) {
        SPDLOG_ERROR("failed to load image: {}", filepath);
        return false;
    }
    return true;
}