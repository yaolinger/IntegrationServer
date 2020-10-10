#ifndef UTILS_STREAM_SOCKET_HPP
#define UTILS_STREAM_SOCKET_HPP

#include <memory>
#include <string>
#include "macros.hpp"
#include "reactor_epoll.hpp"
#include "reactor_unit.hpp"
#include "scheduler_unit.hpp"

NS_UTILS_BEGIN

// reactor挂载数据
class ReactorSocketData;
typedef std::shared_ptr<ReactorSocketData> ReactorSocketDataPtr;

// 错误数据
#define INVAILD_ERROR -1

// 关闭socket类型
enum SHUTDOWN_TYPE {
    SHUTDOWN_TYPE_READ = 0,
    SHUTDOWN_TYPE_WRITE = 1,
    SHUTDOWN_TYPE_ALL = 2,
};

// socket(数据流)
class StreamSocket {
public:
    StreamSocket(ReactorEpollPtr pReacotr);
    StreamSocket(int32 fd, ReactorEpollPtr pReactor);
    ~StreamSocket();

public:
    // 获取socket
    int32 getFd() { return m_fd; }
    // 获取错误信息
    std::string getError() { return m_error; }
    // 清除错误信息
    void clearError() { m_error.clear(); }

// 异步操作
public:
    // 异步发起链接
    void asynConnect(const std::string& ip, uint16 port, ReactorUnitPtr pRUnit);
    // 异步发起接受
    void asynAccept(int32& fd, std::string& ip, uint16& port, ReactorUnitPtr pRUnit);
    // 异步接受数据
    void asynRecvData(char* buf, uint32 len, ReactorUnitPtr pRUnit);
    // 异步发送数据
    void asynSendData(const char* buf, uint32 len, ReactorUnitPtr pRUnit);

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
    // 关闭socket
    bool closeSocket();
    // 关闭socket
    bool shutDownSocket(SHUTDOWN_TYPE type);
    // 取消units
    void cancelUnits();

private:
    int32 m_fd;                              // socket
    bool m_first;                            // 首次注册
    ReactorSocketDataPtr  m_rsd;             // reactor 挂载数据
    std::string           m_error;           // 错误数据
    ReactorEpollPtr       m_reactorEpoll;    // reacotr(暂时不抽象)
};

NS_UTILS_END

#endif
