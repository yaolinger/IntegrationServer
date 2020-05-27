#ifndef BOOST_NETWORK_MESSAGE_HPP
#define BOOST_NETWORK_MESSAGE_HPP

#include <cstring>
#include <memory>

#include "utils/macros.hpp"

NS_BOOST_NETWORK_BEGIN

// 消息对象
// 取消拷贝构造 拷贝赋值操作符
// 支持c++11 移动语义 移动构造 移动赋值操作符
class MsgBuf {
public:
    explicit MsgBuf(uint32 len, bool isCompress = false) : m_size(len), m_isCompress(isCompress) {
        m_data = new char[len];
    }

    explicit MsgBuf(const char* pData, uint32 len, bool isCompress) : m_size(len), m_isCompress(isCompress) {
        m_data = new char[len];
        if (NULL != pData) {
            std::memcpy(this->m_data, pData, len);
        }
    }

    MsgBuf() = delete;
    MsgBuf(const MsgBuf&) = delete;
    MsgBuf& operator = (const MsgBuf&) = delete;

    ~MsgBuf() {
        if (NULL != m_data) {
            delete [] m_data;
            m_data = NULL;
        }
    }

public:
    char* data() const { return m_data; }
    uint32 size() const { return m_size; }
    bool isCompress() const { return m_isCompress; }

private:
    char* m_data = NULL;            // 数据
    uint32 m_size = 0;              // 数据长度
    bool m_isCompress = false;      // 是否压缩
};

typedef std::shared_ptr<MsgBuf> MsgBufPtr;

NS_BOOST_NETWORK_END

#endif
