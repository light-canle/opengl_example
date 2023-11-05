#include "buffer.h"

BufferUPtr Buffer::CreateWithData(uint32_t bufferType, uint32_t usage,
    const void* data, size_t dataSize) 
{
    auto buffer = BufferUPtr(new Buffer());
    if (!buffer->Init(bufferType, usage, data, dataSize))
        return nullptr;
    return std::move(buffer);
}

// 버퍼 삭제
Buffer::~Buffer() {
    if (m_buffer) {
        glDeleteBuffers(1, &m_buffer);
    }
}

// 버퍼 바인딩 - 타입 지정
void Buffer::Bind() const {
    glBindBuffer(m_bufferType, m_buffer);
}

bool Buffer::Init(uint32_t bufferType, uint32_t usage, const void* data, size_t dataSize) {
    m_bufferType = bufferType;
    m_usage = usage;
    // 버퍼 생성 / 바인딩
    glGenBuffers(1, &m_buffer);
    Bind();
    // 데이터를 넣어줌
    glBufferData(m_bufferType, dataSize, data, usage);
    return true;
}