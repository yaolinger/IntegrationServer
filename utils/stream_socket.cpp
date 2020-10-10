#include "stream_socket.hpp"

#include <arpa/inet.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include "error.hpp"
#include "log.hpp"
#include "reactor_epoll_mount_data.hpp"

NS_UTILS_BEGIN

StreamSocket::StreamSocket(ReactorEpollPtr pReacotr) {
    m_fd = ::socket(AF_INET, SOCK_STREAM, 0);
    if (INVAILD_ERROR == m_fd) {
        m_error = GET_SYSTEM_ERRNO_INFO;
        return;
    }
    m_mount = std::make_shared<ReactorEpollMountData>(m_fd, pReacotr);
}

StreamSocket::StreamSocket(int32 fd, ReactorEpollPtr pReacotr) : m_fd(fd) {
    m_mount = std::make_shared<ReactorEpollMountData>(m_fd, pReacotr);
}

StreamSocket::~StreamSocket() {
    m_mount.reset();
}

void StreamSocket::asynConnect(const std::string& ip, uint16 port, ReactorUnitPtr pRUnit) {
    auto netFunc = [this](ReactorUnitPtr ptr, std::string& ip, uint16& port) {
        if (!this->synConnect(ip, port)) {
            ptr->m_error = GET_SYSTEM_ERRNO_INFO;
        }
        ptr->complete();
    };
    // 直接提交执行
    SchdeulerUnitPtr pNetUnit = std::make_shared<SchdeulerUnit>(std::bind(netFunc, pRUnit, ip, port));
    m_mount->post(pNetUnit);
}

void StreamSocket::asynAccept(int32& fd, std::string& ip, uint16& port, ReactorUnitPtr pRUnit) {
    // 构建 netFunc : IOFunc(网络io) + ReactorUnitPtr(逻辑回调)
    auto netFunc = [this](ReactorUnitPtr ptr, int32& fd, std::string& ip, uint16& port) {
        // 构建IOFunc
        auto IOFunc = [this](ReactorUnitPtr ptr, int32& fd, std::string& ip, uint16& port) {
            fd = this->synAccept(ip, port);
            if (fd == INVAILD_ERROR) {
                if (errno == EAGAIN) {
                    log_warn("Accept socket EAGAIN");
                    return false;
                }
                ptr->m_error = GET_SYSTEM_ERRNO_INFO;
            }
            return true;
        };

        // 执行IOFunc
        if (!this->m_mount->runIOFunc(REACTOR_EVENT_READ, std::bind(IOFunc, ptr, std::ref(fd), std::ref(ip), std::ref(port)))) {
            // IOFunc执行失败(EAGAIN),重新发起异步接收
            this->asynAccept(fd, ip, port, ptr);
        } else {
            // 执行ReactorUnitPtr(逻辑回调)
            ptr->complete();
        }
    };
    // 构建netUnit
    SchdeulerUnitPtr pNetUnit = std::make_shared<SchdeulerUnit>(std::bind(netFunc, pRUnit, std::ref(fd), std::ref(ip), std::ref(port)));
    m_mount->startEvent(REACTOR_EVENT_READ, pNetUnit);
}

void StreamSocket::asynRecvData(char* buf, uint32 len, ReactorUnitPtr pRUnit) {
    auto netFunc = [this](char* buf, uint32 len, ReactorUnitPtr ptr) {
        // 构建IOFunc
        auto IOFunc = [this](char* buf, uint32 len, ReactorUnitPtr ptr) {
            int32 ret = this->recvData(buf, len);
            if (ret == INVAILD_ERROR) {
                if (errno == EAGAIN) {
                    log_warn("Asyn recv EAGAIN.");
                    return false;
                }
                ptr->m_error = GET_SYSTEM_ERRNO_INFO;
            }
            ptr->m_transBytes = ret;
            return true;
        };
        if (!this->m_mount->runIOFunc(REACTOR_EVENT_READ, std::bind(IOFunc, buf, len, ptr))) {
            this->asynRecvData(buf, len, ptr);
        } else {
            ptr->complete();
        }
    };

    // 构建netUnit
    SchdeulerUnitPtr pNetUnit = std::make_shared<SchdeulerUnit>(std::bind(netFunc, buf, len, pRUnit));
    m_mount->startEvent(REACTOR_EVENT_READ, pNetUnit);
}

void StreamSocket::asynSendData(const char* buf, uint32 len, ReactorUnitPtr pRUnit) {
    auto netFunc = [this](const char* buf, uint32 len, ReactorUnitPtr ptr) {
        // 构建IOFunc
        auto IOFunc = [this](const char* buf, uint32 len, ReactorUnitPtr ptr) {
            int32 ret = this->sendData(buf, len);
            if (ret == INVAILD_ERROR) {
                if (errno == EAGAIN) {
                    log_warn("Asyn send EAGAIN");
                    return false;
                }
                ptr->m_error = GET_SYSTEM_ERRNO_INFO;
            }
            ptr->m_transBytes = ret;
            return true;
        };
        if (!this->m_mount->runIOFunc(REACTOR_EVENT_WRITE, std::bind(IOFunc, buf, len, ptr))) {
            this->asynSendData(buf, len, ptr);
        } else {
            ptr->complete();
        }
    };

    // 构建netUnit
    // 默认先注册读 写事件直接添加
    SchdeulerUnitPtr pNetUnit = std::make_shared<SchdeulerUnit>(std::bind(netFunc, buf, len, pRUnit));
    m_mount->startEvent(REACTOR_EVENT_WRITE, pNetUnit);
}

void StreamSocket::reUse() {
    int32 on = 1;
    ::setsockopt(m_fd, SOL_SOCKET, SO_REUSEADDR, (char*)&on, sizeof(on));
    ::setsockopt(m_fd, SOL_SOCKET, SO_REUSEPORT, (char*)&on, sizeof(on));
}

bool StreamSocket::setNonblock() {
    clearError();
    int32 oldFlag = ::fcntl(m_fd, F_GETFL, 0);
    int32 newFlag = oldFlag | O_NONBLOCK;
    if (::fcntl(m_fd, F_SETFL, newFlag) == INVAILD_ERROR) {
        m_error = GET_SYSTEM_ERRNO_INFO;
        return false;
    }
    return true;
}

bool StreamSocket::bind(const std::string& ip, uint16 port) {
    clearError();
    struct sockaddr_in bindAddr;
    bindAddr.sin_family = AF_INET;
    bindAddr.sin_addr.s_addr = inet_addr(ip.c_str());
    bindAddr.sin_port = htons(port);
    if (::bind(m_fd, (sockaddr *)&bindAddr, sizeof(bindAddr)) == INVAILD_ERROR) {
        m_error = GET_SYSTEM_ERRNO_INFO;
        return false;
    }
    return true;
}

bool StreamSocket::listen() {
    clearError();
    if (::listen(m_fd, SOMAXCONN) == INVAILD_ERROR) {
        m_error = GET_SYSTEM_ERRNO_INFO;
        return false;
    }
    return true;
}

bool StreamSocket::synConnect(const std::string& ip, uint16 port) {
    clearError();
    struct sockaddr_in connectAddr;
    connectAddr.sin_family = AF_INET;
    connectAddr.sin_addr.s_addr = inet_addr(ip.c_str());
    connectAddr.sin_port = htons(port);
    if (::connect(m_fd, (sockaddr*)&connectAddr, sizeof(connectAddr)) == INVAILD_ERROR) {
        m_error = GET_SYSTEM_ERRNO_INFO;
        return false;
    }
    return true;
}

int32 StreamSocket::synAccept(std::string& ip, uint16& port) {
    clearError();
    struct sockaddr_in clientAddr;
    socklen_t clientAddrLen = sizeof(clientAddr);
    int32 clientFd = accept(m_fd, (sockaddr*)&clientAddr, &clientAddrLen);
    if (clientFd != INVAILD_ERROR) {
        port = ntohs(clientAddr.sin_port);
        char str[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &(clientAddr.sin_addr), str, sizeof(str));
        ip = str;
    }
    return clientFd;
}

int32 StreamSocket::sendData(const char* buf, int32 len) {
    return ::send(m_fd, buf, len, 0);
}

int32 StreamSocket::recvData(char* buf, int32 len) {
    return ::recv(m_fd, buf, len, 0);
}

bool StreamSocket::closeSocket() {
    if (::close(m_fd) == INVAILD_ERROR) {
        m_error = GET_SYSTEM_ERRNO_INFO;
        return false;
    }
    return true;
}

bool StreamSocket::shutDownSocket(SHUTDOWN_TYPE type) {
    if (::shutdown(m_fd, type) == INVAILD_ERROR) {
        m_error = GET_SYSTEM_ERRNO_INFO;
        return false;
    }
    return true;
}

void StreamSocket::cancelUnits() {
    // 取消units
    m_mount->cancelUnits();
}

NS_UTILS_END
