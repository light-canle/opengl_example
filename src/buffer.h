#ifndef __BUFFER_H__
#define __BUFFER_H__

#include "common.h"

CLASS_PTR(Buffer)
class Buffer {
public:
    static BufferUPtr CreateWithData(
        uint32_t bufferType, uint32_t usage,
        const void* data, size_t dataSize);
    ~Buffer();
    uint32_t Get() const { return m_buffer; }
    void Bind() const;

private:
    Buffer() {}
    bool Init(
        uint32_t bufferType, uint32_t usage,
        const void* data, size_t dataSize);
    // 버퍼 ID
    uint32_t m_buffer { 0 };
    // 버퍼 종류(VBO, EBO)
    uint32_t m_bufferType { 0 };
    // 그리기 방식 (STATIC, STREAM, DYNAMIC | )
    uint32_t m_usage { 0 };
};

#endif // __BUFFER_H__