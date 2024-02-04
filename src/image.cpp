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
    stbi_set_flip_vertically_on_load(true); // 상하 반전으로 이미지를 원래대로 나타나게 함
    m_data = stbi_load(filepath.c_str(), &m_width, &m_height, &m_channelCount, 0);
    if (!m_data) {
        SPDLOG_ERROR("failed to load image: {}", filepath);
        return false;
    }
    return true;
}

// width x height 크기의 이미지 데이터를 만들어 냄
ImageUPtr Image::Create(int width, int height, int channelCount) {
    auto image = ImageUPtr(new Image());
    if (!image->Allocate(width, height, channelCount))
        return nullptr;
    return std::move(image);
}

// 이미지가 들어갈 공간 할당
bool Image::Allocate(int width, int height, int channelCount) {
    m_width = width;
    m_height = height;
    m_channelCount = channelCount;
    // 메모리 할당
    m_data = (uint8_t*)malloc(m_width * m_height * m_channelCount);
    return m_data ? true : false;
}

// 각 칸의 크기가 gridX * gridY가 되는 흑백 체커 보드를 만들어낸다.
void Image::SetCheckImage(int gridX, int gridY) {
    for (int j = 0; j < m_height; j++) {
        for (int i = 0; i < m_width; i++) {
            int pos = (j * m_width + i) * m_channelCount;
            bool even = ((i / gridX) + (j / gridY)) % 2 == 0;
            uint8_t value = even ? 255 : 0; // 흰색 아니면 검은색
            for (int k = 0; k < m_channelCount; k++)
                m_data[pos + k] = value;
            // 알파값은 255로 설정
            if (m_channelCount > 3)
                m_data[3] = 255;
        }
    }
}

// width x height 크기의 color의 색을 가진 단색 이미지를 생성한다.
ImageUPtr Image::CreateSingleColorImage(int width, int height, const glm::vec4& color) {
    // 0~1의 float 색상 값을 uint8 형태로 바꾼다.
    glm::vec4 clamped = glm::clamp(color * 255.0f, 0.0f, 255.0f);
    uint8_t rgba[4] = {
        (uint8_t)clamped.r, 
        (uint8_t)clamped.g, 
        (uint8_t)clamped.b, 
        (uint8_t)clamped.a, 
    };
    // 각 픽셀마다 같은 색을 넣어준다.
    auto image = Create(width, height, 4);
    for (int i = 0; i < width * height; i++) {
        memcpy(image->m_data + 4 * i, rgba, 4);
    }
    return std::move(image);
}