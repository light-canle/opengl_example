#ifndef __IMAGE_H__
#define __IMAGE_H__

#include "common.h"

CLASS_PTR(Image)
class Image {
public:
    // 이미지 로드
    static ImageUPtr Load(const std::string& filepath);
    // 이미지 생성
    static ImageUPtr Create(int width, int height, int channelCount = 4);
    ~Image();

    const uint8_t* GetData() const { return m_data; } // 데이터 반환
    int GetWidth() const { return m_width; }
    int GetHeight() const { return m_height; }
    int GetChannelCount() const { return m_channelCount; }

    // 체커 이미지 생성
    void SetCheckImage(int gridX, int gridY);

private:
    Image() {};
    bool LoadWithStb(const std::string& filepath); // 이미지 로드
    bool Allocate(int width, int height, int channelCount); // 이미지 생성을 위한 메모리 할당
    int m_width { 0 }; // 가로 길이
    int m_height { 0 }; // 세로 길이
    int m_channelCount { 0 }; // 채널 수
    uint8_t* m_data { nullptr };
};

#endif // __IMAGE_H__