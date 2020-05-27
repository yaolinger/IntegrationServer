#include "buffer.hpp"

NS_BOOST_NETWORK_BEGIN

template<>
DynamicBuffer::Buffer()
    : m_maxSize(DYNAMIC_BUFF_BASE_SIZE),
      m_rearIndex(0),
      m_curIndex(0),
      m_buffer(DYNAMIC_BUFF_BASE_SIZE) {
}

template<>
void DynamicBuffer::reserve(uint32 size) {
    if (getWriteSize() < size) {
        m_maxSize += DYNAMIC_BUFF_BASE_SIZE*(DYNAMIC_BUFF_EXPAND_TIMES(size));
        m_buffer.resize(m_maxSize);
    }
}

template<>
StaticBuffer::Buffer()
    : m_maxSize(DYNAMIC_BUFF_BASE_SIZE),
      m_rearIndex(0),
      m_curIndex(0) {
}

template<>
void StaticBuffer::reserve(uint32 size) {
}

NS_BOOST_NETWORK_END
