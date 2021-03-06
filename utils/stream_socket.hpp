#ifndef UTILS_STREAM_SOCKET_HPP
#define UTILS_STREAM_SOCKET_HPP

#include <memory>
#include <string>
#include "error.hpp"
#include "log.hpp"
#include "handler_check_socket.hpp"
#include "macros.hpp"
#include "reactor_epoll.hpp"
#include "reactor_epoll_mount_data.hpp"
#include "reactor_unit.hpp"
#include "scheduler_unit.hpp"

NS_UTILS_BEGIN

// 错误数据
#define INVAILD_ERROR -1

// 关闭socket类型
enum SHUTDOWN_TYPE {
    SHUTDOWN_TYPE_READ = 0,
    SHUTDOWN_TYPE_WRITE = 1,
    SHUTDOWN_TYPE_ALL = 2,
};

// socket(数据流)
class StreamSocket : public std::enable_shared_from_this<StreamSocket> {
public:
    StreamSocket(ReactorEpollPtr pReacotr);
    StreamSocket(int32 fd, ReactorEpollPtr pReactor);
    ~StreamSocket();

public:
    // 获取socket
    int32 getFd() { return m_fd; }
    // 获取错误
    std::string getError() { return GET_SYSTEM_ERRNO_INFO; }

public:
    // 设置地址重用
    void reUseAddr();
	// 端口重用
	void reUsePort();
	// 关闭延迟响应
	void quickAck();
	// 关闭nagle算法
	void noDelay();
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

// 异步操作重构(function, functor都可)
public:
    // 异步connect(Handler(const std::string error))
    template <typename Handler>
    void asyncConnect(const std::string& ip, uint16 port, Handler handler);

    // 异步accept(Handler(const std::string error))
    template <typename Handler>
    void asyncAccept(int32& fd, std::string& ip, uint16& port, Handler handler);

    // 异步读数据(Handler(const std::string error, int32 transBytes))
    template <typename Handler>
    void asyncReadData(char* buf, uint32 len, Handler handler);

    // 异步写数据(Handler(const std::string error, int32 transBytes))
    template <typename Handler>
    void asyncWriteData(const char* buf, uint32 len, Handler handler);

public:
    int32 m_fd;                              // socket
    ReactorEpollMountDataPtr  m_mount;       // reactor 挂载数据
};

// 智能指针
typedef std::shared_ptr<StreamSocket> StreamSocketPtr;

template <typename Handler>
void StreamSocket::asyncConnect(const std::string& ip, uint16 port, Handler handler) {
    // 校验handler 参数
    ASYNC_CONNECT_HANDLER_CHECK(handler);
    // 构建netFunc
    auto netFunc = [] (StreamSocketPtr ptr, const std::string& ip, uint16 port, Handler handler) {
        std::string error;
        if (!ptr->synConnect(ip, port)) {
            error = GET_SYSTEM_ERRNO_INFO;
        }
        handler(error);
    };
    // 提交
    SchdeulerUnitPtr pNetUnit = std::make_shared<SchdeulerUnit>(std::bind(netFunc, shared_from_this(), std::ref(ip), port, handler));
    m_mount->post(pNetUnit);
}

template <typename Handler>
void StreamSocket::asyncAccept(int32& fd, std::string& ip, uint16& port, Handler handler) {
    // 校验handler 参数
    ASYNC_ACCEPT_HANDLER_CHECK(handler);
    // 构建netFunc
    auto netFunc = [] (StreamSocketPtr ptr, int32& fd, std::string& ip, uint16& port, Handler handler) {
        // 构建IOFunc
        auto IOFunc = [] (StreamSocketPtr ptr, int32& fd, std::string& ip, uint16& port, std::string& error) {
            fd = ptr->synAccept(ip, port);
            if (fd == INVAILD_ERROR) {
                if (errno == EAGAIN) {
                    log_warn("Accept socket EAGAIN");
                    return false;
                }
                error = GET_SYSTEM_ERRNO_INFO;
            }
            return true;
        };
        // 执行IOFunc
        std::string error;
        if (!ptr->m_mount->runIOFunc(REACTOR_EVENT_READ, std::bind(IOFunc, ptr, std::ref(fd), std::ref(ip), std::ref(port), std::ref(error)))) {
            // IOFunc执行失败(EAGAIN),重新发起异步接收
            ptr->asyncAccept(fd, ip, port, handler);
        } else {
            handler(error);
        }
    };

    // 构建netUnit
    SchdeulerUnitPtr pNetUnit = std::make_shared<SchdeulerUnit>(std::bind(netFunc, shared_from_this(), std::ref(fd), std::ref(ip), std::ref(port), handler));
    m_mount->startEvent(REACTOR_EVENT_READ, pNetUnit);
}

template <typename Handler>
void StreamSocket::asyncReadData(char* buf, uint32 len, Handler handler) {
    // 校验handler 参数
    ASYNC_READ_HANDLER_CHECK(handler);
    // 构建netFunc
    auto netFunc = [] (StreamSocketPtr ptr, char* buf, uint32 len, Handler handler) {
        // 构建IOFunc
        auto IOFunc = [] (StreamSocketPtr ptr, char* buf, uint32 len, std::string& error, int32& bytes) {
            int32 ret = ptr->recvData(buf, len);
            if (ret == INVAILD_ERROR) {
                if (errno == EAGAIN) {
                    log_warn("Asyn recv EAGAIN.");
                    return false;
                }
                error = GET_SYSTEM_ERRNO_INFO;
            }
            bytes = ret;
            return true;
        };
        // 执行IOFunc
        std::string error;
        int32 bytes;
        if (!ptr->m_mount->runIOFunc(REACTOR_EVENT_READ, std::bind(IOFunc, ptr, buf, len, std::ref(error), std::ref(bytes)))) {
            ptr->asyncReadData(buf, len, handler);
        } else {
            handler(error, bytes);
        }
    };

    // 构建netUnit
    SchdeulerUnitPtr pNetUnit = std::make_shared<SchdeulerUnit>(std::bind(netFunc, shared_from_this(), buf, len, handler));
    m_mount->startEvent(REACTOR_EVENT_READ, pNetUnit);
}

template <typename Handler>
void StreamSocket::asyncWriteData(const char* buf, uint32 len, Handler handler) {
    // 校验handler 参数
    ASYNC_WRITE_HANDLER_CHECK(handler);
    // 构建netFunc
    auto netFunc = [] (StreamSocketPtr ptr, const char* buf, uint32 len, Handler handler) {
        // 构建IOFunc
        auto IOFunc = [] (StreamSocketPtr ptr, const char* buf, uint32 len, std::string& error, int32& bytes) {
            int32 ret = ptr->sendData(buf, len);
            if (ret == INVAILD_ERROR) {
                if (errno == EAGAIN) {
                    log_warn("Asyn send EAGAIN");
                    return false;
                }
                error = GET_SYSTEM_ERRNO_INFO;
            }
			log_info("Send data[%d] success", ret);
            bytes = ret;
            return true;
        };
        // 执行IOFunc
        std::string error;
        int32 bytes;
        if (!ptr->m_mount->runIOFunc(REACTOR_EVENT_WRITE, std::bind(IOFunc, ptr, buf, len, std::ref(error), std::ref(bytes)))) {
            ptr->asyncWriteData(buf, len, handler);
        } else {
            handler(error, bytes);
        }
    };

    // 构建netUnit
    SchdeulerUnitPtr pNetUnit = std::make_shared<SchdeulerUnit>(std::bind(netFunc, shared_from_this(), buf, len, handler));
    m_mount->startEvent(REACTOR_EVENT_WRITE, pNetUnit);
}

NS_UTILS_END

#endif
