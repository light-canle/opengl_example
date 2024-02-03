#ifndef __BUFFER_H__
#define __BUFFER_H__

#include "common.h"

CLASS_PTR(Buffer)
class Buffer {
public:
    static BufferUPtr CreateWithData(
        uint32_t bufferType, uint32_t usage,
        const void* data, size_t stride, size_t count);
    ~Buffer();
    uint32_t Get() const { return m_buffer; }
    size_t GetStride() const { return m_stride; }
    size_t GetCount() const { return m_count; }
    void Bind() const;

private:
    Buffer() {}
    bool Init(
        uint32_t bufferType, uint32_t usage,
        const void* data, size_t stride, size_t count);
    // 버퍼 ID
    uint32_t m_buffer { 0 };
    // 버퍼 종류(VBO, EBO)
    uint32_t m_bufferType { 0 };
    // 그리기 방식 (STATIC, STREAM, DYNAMIC | )
    uint32_t m_usage { 0 };
    // 정점 데이터 1개의 크기(간격)
    size_t m_stride { 0 };
    // 정점 데이터 개수
    size_t m_count { 0 };
};

#endif // __BUFFER_H__