#ifndef __IMAGE_H__
#define __IMAGE_H__

#include "common.h"

CLASS_PTR(Image)
class Image {
public:
    // 이미지 로드
    static ImageUPtr Load(const std::string& filepath, bool flipVertical = true);
    // 이미지 생성
    static ImageUPtr Create(int width, int height, int channelCount = 4, int bytePerChannel = 1);
    ~Image();

    // 단색으로만 이루어진 이미지를 생성
    static ImageUPtr CreateSingleColorImage(int width, int height, const glm::vec4& color);

    const uint8_t* GetData() const { return m_data; } // 데이터 반환
    int GetWidth() const { return m_width; }
    int GetHeight() const { return m_height; }
    int GetChannelCount() const { return m_channelCount; }
    int GetBytePerChannel() const { return m_bytePerChannel; }

    // 체커 이미지 생성
    void SetCheckImage(int gridX, int gridY);

private:
    Image() {};
    bool LoadWithStb(const std::string& filepath, bool flipVertical); // 이미지 로드
    bool Allocate(int width, int height, int channelCount, int bytePerChannel); // 이미지 생성을 위한 메모리 할당
    int m_width { 0 }; // 가로 길이
    int m_height { 0 }; // 세로 길이
    int m_channelCount { 0 }; // 채널 수
    int m_bytePerChannel { 1 }; // 채널 당 바이트 수
    uint8_t* m_data { nullptr };
};

#endif // __IMAGE_H__