#ifndef TEST_UTILS_TEST_STREAM_SOCKET_HPP
#define TEST_UTILS_TEST_STREAM_SOCKET_HPP

#include <atomic>
#include <string>
#include "utils/macros.hpp"
#include "utils/rand.hpp"
#include "utils/reactor_epoll.hpp"
#include "utils/reactor_timer.hpp"
#include "utils/stream_socket.hpp"
#include "utils/log.hpp"

// 读取缓存区大小
#define READ_CACHE_LEN 10240

// socket
class NetworkSocket {
public:
    NetworkSocket() {
        m_closeSign.store(false);
        m_writeSign.store(false);
    }
    NetworkSocket(UTILS::ReactorEpollPtr pReactor, std::shared_ptr<UTILS::StreamSocket> pSS, std::string ip, uint16 port) : m_sSocket(pSS) , m_port(port), m_ip(std::move(ip)) {
        m_closeSign.store(false);
        m_writeSign.store(false);
        m_connectTimer = std::make_shared<UTILS::ReactorTimer>(pReactor);
    }
    ~NetworkSocket() {
        if (m_sSocket) {
            m_sSocket.reset();
        }
    }

    void init(UTILS::ReactorEpollPtr pReactor, std::shared_ptr<UTILS::StreamSocket> pSS, std::string ip, uint16 port) {
        m_connectTimer = std::make_shared<UTILS::ReactorTimer>(pReactor);
        m_sSocket = pSS;
        m_port = port;
        m_ip = ip;
    }

    void connect() {
        m_sSocket->setNonblock();
        auto func = [&](const std::string error, int32 transBytes) {
            if (error.size() == 0) {
                log_info("Connect[%s:%u] successful~~~.", this->m_ip.c_str(), this->m_port);
                this->recv();
            } else {
                log_error("Connect error, %s", error.c_str());
                this->reconnect();
            }
        };
        UTILS::ReactorUnitPtr pRUnit = std::make_shared<UTILS::ReactorUnit>(func);
        m_sSocket->asynConnect(m_ip, m_port, pRUnit);
    }

    void reconnect() {
        m_connectTimer->expiresFunc(1, [this](){
                log_info("Reconnect[%s:%u] ........", this->m_ip.c_str(), this->m_port);
                this->connect();
            });
    }

public:
    void recv() {
        auto func = [&](const std::string error, int32 transBytes) {
            if (error.size() > 0) {
                log_error("Recv data error, %s", error.c_str());
                return;
            }
            if (transBytes == 0) {
                log_info("Socket[%d] close.", this->m_sSocket->getFd());
                this->close();
                return;
            }

            log_info("Recv socket[%d] data[%d:%s]", this->m_sSocket->getFd(), transBytes, this->m_readBytes);
            this->recv();
        };
        UTILS::ReactorUnitPtr pRUnit = std::make_shared<UTILS::ReactorUnit>(func);
        memset(m_readBytes, 0, READ_CACHE_LEN);
        m_sSocket->asynRecvData(m_readBytes, READ_CACHE_LEN, pRUnit);
    }

    void send(std::string buf) {
        if (m_closeSign.load()) {
            return;
        }

        for (uint32 i = 0; i < buf.size(); i++) {
            m_writeBytes.push_back(buf[i]);
        }

        // 正在写数据
        if (m_writeSign.load()) {
            return;
        }

        m_writeSign.store(true);

        auto func = [&](const std::string error, int32 transBytes) {
            if (error.size() > 0) {
                log_error("Send data error,%s", error.c_str());
                return;
            }
            log_info("Send socket[%d] data[%d]", this->m_sSocket->getFd(), transBytes);
            for (int32 i = 0; i< transBytes && m_writeBytes.size() > 0; i++) {
                m_writeBytes.erase(m_writeBytes.begin());
            }

            this->m_writeSign.store(false);
            if (m_writeBytes.size() > 0) {
                this->send(std::string(""));
            }
        };
        UTILS::ReactorUnitPtr pRUnit = std::make_shared<UTILS::ReactorUnit>(func);
        m_sSocket->asynSendData(&m_writeBytes[0], m_writeBytes.size(), pRUnit);
    }

    void close() {
        m_closeSign.store(true);
        m_sSocket->closeSocket();
        m_sSocket->cancelUnits();
    }

private:
    std::shared_ptr<UTILS::StreamSocket> m_sSocket;
    uint16 m_port;
    std::string m_ip;
    std::atomic<bool> m_closeSign;                            // 关闭标识
    char m_readBytes[READ_CACHE_LEN];                         // 读缓存区
    std::atomic<bool> m_writeSign;                            // 写标识
    std::vector<char> m_writeBytes;                           // 写缓存区
    std::shared_ptr<UTILS::ReactorTimer> m_connectTimer;      // 重连定时器
};

typedef std::shared_ptr<NetworkSocket> NSocketPtr;

// accept
class NetworkAccept {
public:
    NetworkAccept(std::shared_ptr<UTILS::StreamSocket> pSS, UTILS::ReactorEpollPtr pReactor) : m_sSocket(pSS), m_newFd(-1), m_eReactor(pReactor) {}
    NetworkAccept() : m_sSocket(NULL), m_newFd(-1) {};
    ~NetworkAccept() {
        if (m_sSocket) {
            m_sSocket.reset();
        }
    }

public:
    void init(std::shared_ptr<UTILS::StreamSocket> pSS, UTILS::ReactorEpollPtr pReacotr) {
        m_sSocket = pSS;
        m_eReactor = pReacotr;
    }

    bool listen(const std::string& ip, uint16 port) {
        m_sSocket->reUse();
        m_sSocket->setNonblock();
        if (!m_sSocket->bind(ip, port)) {
            log_error("Bind error[%s]", m_sSocket->getError().c_str());
            return false;
        }
        if (!m_sSocket->listen()) {
            log_error("Error[%s]", m_sSocket->getError().c_str());
            return false;
        }
        log_info("Server listen[%s:%u:%d] start~~~~~ ", ip.c_str(), port, m_sSocket->getFd());
        return true;
    }

    void accept() {
        auto func = [&](const std::string error, int32 transBytes) {
            if (m_newFd == -1) {
                log_error("Accept error[%s]", error.c_str());
                return;
            }
            log_info("New socket[%s:%u:%d] on thread[%ld]", m_newIp.c_str(), m_newPort, m_newFd, std::this_thread::get_id());
            //std::this_thread::sleep_for(std::chrono::milliseconds(10));
            std::shared_ptr<UTILS::StreamSocket> pSSocket = std::make_shared<UTILS::StreamSocket>(m_newFd, m_eReactor);
            pSSocket->setNonblock();
            NSocketPtr pNSocket = std::make_shared<NetworkSocket>(m_eReactor, pSSocket, m_newIp, m_newPort);
            this->m_socketMap[m_newFd] = pNSocket;
            pNSocket->recv();
            this->accept();
        };
        UTILS::ReactorUnitPtr pRUnit = std::make_shared<UTILS::ReactorUnit>(func);
        m_sSocket->asynAccept(m_newFd, m_newIp, m_newPort, pRUnit);
    }

private:
    std::shared_ptr<UTILS::StreamSocket> m_sSocket;
    int32 m_newFd;
    uint16 m_newPort;
    std::string m_newIp;
    UTILS::ReactorEpollPtr m_eReactor;
    std::map<int32, NSocketPtr> m_socketMap;
};

#endif
