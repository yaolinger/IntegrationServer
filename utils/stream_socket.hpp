#ifndef UTILS_STREAM_SOCKET_HPP
#define UTILS_STREAM_SOCKET_HPP

#include "macros.hpp"

#include <memory>
#include <string>

NS_UTILS_BEGIN

// reactor挂载数据
class ReactorSocketData;
typedef std::shared_ptr<ReactorSocketData> ReactorSocketDataPtr;

// 错误数据
#define INVAILD_ERROR -1

// socket(数据流)
class StreamSocket {
public:
    StreamSocket();
    StreamSocket(int32 fd);
    ~StreamSocket();

public:
    // 获取socket
    int32 getFd() { return m_fd; }
    // 获取错误信息
    std::string getError() { return m_error; }
    // 清除错误信息
    void clearError() { m_error.clear(); }

// TODO: 异步操作(没想好)
public:
    // 异步发起链接
    void asynConnect();
    // 异步发起接受
    void asynAccept();
    // 异步发送数据
    void asynSendData();
    // 异步接受数据
    void asynRecvData();

public:
    // 设置地址重用
    void reUse();
    // 设置非阻塞
    bool setNonblock();
    // 绑定地址
    bool bind(const std::string& ip, uint16 port);
    // 开始监听
    bool listen();
    // 发起链接
    bool synConnect(const std::string& ip, uint16 port);
    // 发起接受
    int32 synAccept(std::string& ip, uint16& port);
    // 发送数据
    int32 sendData(const char* buf, int32 len);
    // 接受数据
    int32 recvData(char* buf, int32 len);

private:
    int32 m_fd;                       // socket
    ReactorSocketDataPtr  m_rsd;      // reactor 挂载数据
    std::string           m_error;    // 错误数据
};

NS_UTILS_END

#endif
