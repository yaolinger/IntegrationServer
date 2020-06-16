#ifndef BOOST_NETWORK_BUFFER_HPP
#define BOOST_NETWROK_BUFFER_HPP

#include <cstring>
#include <vector>
#include "common.hpp"
#include "utils/log.hpp"
#include "utils/macros.hpp"

NS_BOOST_NETWORK_BEGIN

// 数据缓存区模板
/**
 * 多读多写,非线程安全
 * 需要配合boost::asio::strand使用
 * 保证数据时序 以及buffer操作安全
 */
template <typename bufferType>
class Buffer {
public:
	Buffer();
	~Buffer() {}

public:
	// 重新分配缓存区大小
	void reserve(uint32 size);

public:
	// 获取可写缓存区位置(允许直接写入)
	char* getWritePos() { return &m_buffer[m_rearIndex]; }
    // 获取可写缓存区大小
	uint32 getWriteSize() { return m_maxSize - m_rearIndex; }
	// 增加写入数据长度
	void addWirteLen(uint32 len) { m_rearIndex += len; }

    // 获取可读缓存区位置
	char* getReadPos() { return &m_buffer[m_curIndex]; }
    // 获取可读缓存区长度
	uint32 getReadSize() { return m_rearIndex - m_curIndex; }
	// 数据是否全部读取
	bool isReadOver() { return m_curIndex >= m_rearIndex; }
    // 增加读取数据长度
  	void addReadLen(uint32 len) { m_curIndex += len; }

    // 调整buffer数据位置
    void refitBuffer() {
        if (isReadOver()) {
            m_curIndex = 0;
            m_rearIndex = 0;
        } else {
            uint32 diff = m_rearIndex - m_curIndex;
            std::memmove(&m_buffer[0], &m_buffer[m_curIndex], diff);
            m_curIndex = 0;
            m_rearIndex = diff;
        }
    }

    void show() {
        log_info("cur[%u] rear[%u] max[%u]", m_curIndex, m_rearIndex, m_maxSize);
    }

private:
	uint32 m_maxSize = 0;   // buffer 长度
	uint32 m_rearIndex = 0; // 尾数据 索引
	uint32 m_curIndex = 0;  // 当前已读数据 索引
	bufferType m_buffer;    // 缓存区
};

// 模板特化

// 动态buffer, 动态扩展buffer长度
typedef Buffer<std::vector<char>> DynamicBuffer;
template<> DynamicBuffer::Buffer();
template<> void DynamicBuffer::reserve(uint32 size);

// 静态buffer, 固定缓冲区长度
typedef Buffer<char[STATIC_BUFF_MAX_SIZE]> StaticBuffer;
template<> StaticBuffer::Buffer();
template<> void StaticBuffer::reserve(uint32 size);

NS_BOOST_NETWORK_END

#endif
