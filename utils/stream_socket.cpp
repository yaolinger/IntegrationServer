#include "stream_socket.hpp"

#include <arpa/inet.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include "error.hpp"

NS_UTILS_BEGIN

StreamSocket::StreamSocket(ReactorEpollPtr pReacotr) {
    m_fd = ::socket(AF_INET, SOCK_STREAM, 0);
    if (INVAILD_ERROR == m_fd) {
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

void StreamSocket::reUse() {
    int32 on = 1;
    ::setsockopt(m_fd, SOL_SOCKET, SO_REUSEADDR, (char*)&on, sizeof(on));
    ::setsockopt(m_fd, SOL_SOCKET, SO_REUSEPORT, (char*)&on, sizeof(on));
}

bool StreamSocket::setNonblock() {
    int32 oldFlag = ::fcntl(m_fd, F_GETFL, 0);
    int32 newFlag = oldFlag | O_NONBLOCK;
    if (::fcntl(m_fd, F_SETFL, newFlag) == INVAILD_ERROR) {
        return false;
    }
    return true;
}

bool StreamSocket::bind(const std::string& ip, uint16 port) {
    CLEAR_SYSTEM_ERRNO;
    struct sockaddr_in bindAddr;
    bindAddr.sin_family = AF_INET;
    bindAddr.sin_addr.s_addr = inet_addr(ip.c_str());
    bindAddr.sin_port = htons(port);
    if (::bind(m_fd, (sockaddr *)&bindAddr, sizeof(bindAddr)) == INVAILD_ERROR) {
        return false;
    }
    return true;
}

bool StreamSocket::listen() {
    CLEAR_SYSTEM_ERRNO;
    if (::listen(m_fd, SOMAXCONN) == INVAILD_ERROR) {
        return false;
    }
    return true;
}

bool StreamSocket::synConnect(const std::string& ip, uint16 port) {
    CLEAR_SYSTEM_ERRNO;
    struct sockaddr_in connectAddr;
    connectAddr.sin_family = AF_INET;
    connectAddr.sin_addr.s_addr = inet_addr(ip.c_str());
    connectAddr.sin_port = htons(port);
    if (::connect(m_fd, (sockaddr*)&connectAddr, sizeof(connectAddr)) == INVAILD_ERROR) {
        return false;
    }
    return true;
}

int32 StreamSocket::synAccept(std::string& ip, uint16& port) {
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
    CLEAR_SYSTEM_ERRNO;
    if (::close(m_fd) == INVAILD_ERROR) {
        return false;
    }
    return true;
}

bool StreamSocket::shutDownSocket(SHUTDOWN_TYPE type) {
    CLEAR_SYSTEM_ERRNO;
    if (::shutdown(m_fd, type) == INVAILD_ERROR) {
        return false;
    }
    return true;
}

void StreamSocket::cancelUnits() {
    // 取消units
    m_mount->cancelUnits();
}

NS_UTILS_END
