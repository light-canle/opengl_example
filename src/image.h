#ifndef __IMAGE_H__
#define __IMAGE_H__

#include "common.h"

CLASS_PTR(Image)
class Image {
public:
    // 이미지 로드
    static ImageUPtr Load(const std::string& filepath);
    ~Image();

    const uint8_t* GetData() const { return m_data; } // 데이터 반환
    int GetWidth() const { return m_width; }
    int GetHeight() const { return m_height; }
    int GetChannelCount() const { return m_channelCount; }

private:
    Image() {};
    bool LoadWithStb(const std::string& filepath);
    int m_width { 0 }; // 가로 길이
    int m_height { 0 }; // 세로 길이
    int m_channelCount { 0 }; // 채널 수
    uint8_t* m_data { nullptr };
};

#endif // __IMAGE_H__