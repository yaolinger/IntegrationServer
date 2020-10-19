#ifndef TEST_UTILS_TEST_STREAM_SOCKET_HPP
#define TEST_UTILS_TEST_STREAM_SOCKET_HPP

#include <atomic>
#include <string>
#include "utils/macros.hpp"
#include "utils/rand.hpp"
#include "utils/reactor_epoll.hpp"
#include "utils/reactor_timer.hpp"
#include "utils/scheduler_safe_unit.hpp"
#include "utils/stream_socket.hpp"
#include "utils/log.hpp"

// 读取缓存区大小
#define READ_CACHE_LEN 102400

enum SOCKET_STATE {
    SOCKET_STATE_OPEN = 0,
    SOCKET_STATE_CLOSE = 1,
};

class NetworkSocket;
typedef std::shared_ptr<NetworkSocket> NSocketPtr;

// socket
class NetworkSocket : public::std::enable_shared_from_this<NetworkSocket> {
public:
    NetworkSocket() {
        m_socketReadState.store(SOCKET_STATE_OPEN);
        m_socketWriteState.store(SOCKET_STATE_OPEN);
        m_writeSign.store(false);
        m_close = false;
    }
    NetworkSocket(UTILS::ReactorEpollPtr pReactor, std::shared_ptr<UTILS::StreamSocket> pSS, UTILS::SchedulerPtr pS, std::function<void(int32 fd)> closeFunc, std::string ip, uint16 port) : m_sSocket(pSS), m_port(port), m_ip(std::move(ip)), m_closeFunc(closeFunc) {
        m_socketReadState.store(SOCKET_STATE_OPEN);
        m_socketWriteState.store(SOCKET_STATE_OPEN);
        m_writeSign.store(false);
        m_close = false;
        m_connectTimer = std::make_shared<UTILS::ReactorTimer>(pReactor);
        m_readSSUP = std::make_shared<UTILS::SchdelerSafeUnit>(*pS.get());
        m_writeSSUP = std::make_shared<UTILS::SchdelerSafeUnit>(*pS.get());
    }
    ~NetworkSocket() {
    }

    void init(UTILS::ReactorEpollPtr pReactor, std::shared_ptr<UTILS::StreamSocket> pSS, UTILS::SchedulerPtr pS, std::function<void(int32 fd)> closeFunc, std::string ip, uint16 port) {
        m_connectTimer = std::make_shared<UTILS::ReactorTimer>(pReactor);
        m_sSocket = pSS;
        m_port = port;
        m_ip = std::move(ip);
        m_closeFunc = closeFunc;
        m_readSSUP = std::make_shared<UTILS::SchdelerSafeUnit>(*pS.get());
        m_writeSSUP = std::make_shared<UTILS::SchdelerSafeUnit>(*pS.get());
    }

    void connect(std::function<void()> callback) {
        m_sSocket->setNonblock();
        auto func = [this, callback](const std::string error) {
            if (error.size() == 0) {
                log_info("Connect[%s:%u] successful~~~.", this->m_ip.c_str(), this->m_port);
                this->recv();
                callback();
            } else {
                log_error("Connect error, %s", error.c_str());
                this->reconnect(callback);
            }
        };
        m_sSocket->asyncConnect(m_ip, m_port, func);
    }

    void reconnect(std::function<void()> callback) {
        m_connectTimer->expiresFunc(1, [this, callback](){
                log_info("Reconnect[%s:%u] ........", this->m_ip.c_str(), this->m_port);
                this->connect(callback);
            });
    }

public:
    void recv() {
        auto func = [&](const std::string error, int32 transBytes) {
            if (m_socketReadState.load() != SOCKET_STATE_OPEN) {
                return;
            }
            if (error.size() > 0) {
                log_error("Recv data error, %s", error.c_str());
                return;
            }
            if (transBytes == 0) {
                log_info("Socket[%d] close.", this->m_sSocket->getFd());
                this->onClose();
                return;
            }

            log_info("Recv socket[%d] data[%d]", this->m_sSocket->getFd(), transBytes);
            this->recv();
        };
        memset(m_readBytes, 0, READ_CACHE_LEN);
        m_sSocket->asyncReadData(m_readBytes, READ_CACHE_LEN, m_readSSUP->wrap(func));
    }

    void send(std::string buf) {
        {
            std::lock_guard<std::mutex> lk(m_writeCacheMutex);
            for (uint32 i = 0; i < buf.size(); i++) {
                m_writeBytes.push_back(buf[i]);
            }
        }

        if (m_writeBytes.size() == 0) {
            return;
        }

        // 正在写数据
        if (m_writeSign.exchange(true)) {
            return;
        }

        auto func = [&](const std::string error, int32 transBytes) {
            if (m_socketWriteState.load() != SOCKET_STATE_OPEN) {
                log_info("Close write func.");
                return;
            }
            if (error.size() > 0) {
                log_error("Send data error,%s", error.c_str());
                return;
            }
            log_info("Send socket[%d] data[%d]", this->m_sSocket->getFd(), transBytes);
            {
                std::lock_guard<std::mutex> lk(m_writeCacheMutex);
                for (int32 i = 0; i< transBytes && m_writeBytes.size() > 0; i++) {
                    m_writeBytes.erase(m_writeBytes.begin());
                }
            }

            this->m_writeSign.store(false);
            this->send(std::string(""));
        };
        m_sSocket->asyncWriteData(&m_writeBytes[0], m_writeBytes.size(), m_writeSSUP->wrap(func));
    }

    void onClose() {
        m_sSocket->cancelUnits();
        onReadClose();
        onWriteClose();
    }

private:
    void onReadClose() {
        m_readSSUP->cancelUnits([this] () {
                this->m_socketReadState.store(SOCKET_STATE_CLOSE);
                this->checkCompleteClose();
            });
    }

    void onWriteClose() {
        m_writeSSUP->cancelUnits([this] () {
                this->m_socketWriteState.store(SOCKET_STATE_CLOSE);
                this->checkCompleteClose();
            });
    }

    void checkCompleteClose() {
        if (m_socketReadState.load() && m_socketWriteState.load()) {
            {
                std::lock_guard<std::mutex> lk(m_closeMutex);
                if (m_close) {
                    return;
                }
                m_close = true;
            }
            // 函数包装正加NSocketPtr生命周期(防止逻辑层删除后内部m_sSocket失效)
            auto doClose = [](NSocketPtr ptr) {
                ptr->doCloseFunc();
            };
            doClose(shared_from_this());
        }
    }

    void doCloseFunc() {
        m_closeFunc(m_sSocket->getFd());
        m_sSocket->closeSocket();
    }

private:
    std::shared_ptr<UTILS::StreamSocket> m_sSocket;
    uint16 m_port;
    std::string m_ip;
    std::shared_ptr<UTILS::ReactorTimer> m_connectTimer;      // 重连定时器

    std::atomic<uint32> m_socketReadState;                    // socket 读状态
    UTILS::SchdelerSafeUnitPtr m_readSSUP;                    // 读安全单元
    char m_readBytes[READ_CACHE_LEN];                         // 读缓存区

    std::atomic<uint32> m_socketWriteState;                   // socket 写状态
    std::atomic<bool> m_writeSign;                            // 写标识
    UTILS::SchdelerSafeUnitPtr m_writeSSUP;                   // 写安全单元
    std::mutex m_writeCacheMutex;                             // 写缓存区锁
    std::vector<char> m_writeBytes;                           // 写缓存区

    bool m_close;                                             // 关闭
    std::mutex m_closeMutex;                                  // 关闭锁
    uint32 m_closeTimes = 0;
    std::function<void(int32 fd)> m_closeFunc;                // 关闭func
};

typedef std::shared_ptr<NetworkSocket> NSocketPtr;

// accept
class NetworkAccept {
public:
    NetworkAccept(std::shared_ptr<UTILS::StreamSocket> pSS, UTILS::ReactorEpollPtr pReactor, UTILS::SchedulerPtr pS) : m_sSocket(pSS), m_newFd(-1), m_pS(pS), m_eReactor(pReactor) {}
    NetworkAccept() : m_sSocket(NULL), m_newFd(-1) {};
    ~NetworkAccept() {
    }

public:
    void init(std::shared_ptr<UTILS::StreamSocket> pSS, UTILS::ReactorEpollPtr pReacotr, UTILS::SchedulerPtr pS) {
        m_sSocket = pSS;
        m_eReactor = pReacotr;
        m_pS = pS;
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
        auto func = [&](const std::string error) {
            if (m_newFd == -1) {
                log_error("Accept error[%s]", error.c_str());
                return;
            }
            log_info("New socket[%s:%u:%d] on thread[%ld]", m_newIp.c_str(), m_newPort, m_newFd, std::this_thread::get_id());
            std::shared_ptr<UTILS::StreamSocket> pSSocket = std::make_shared<UTILS::StreamSocket>(m_newFd, m_eReactor);
            pSSocket->setNonblock();
            NSocketPtr pNSocket = std::make_shared<NetworkSocket>(m_eReactor, pSSocket, m_pS, [this](int32 fd) { this->delSocket(fd); }, m_newIp, m_newPort);
            addSocket(m_newFd, pNSocket);
            pNSocket->recv();
            this->accept();
        };
        m_sSocket->asyncAccept(m_newFd, m_newIp, m_newPort, func);
    }

    void addSocket(int32 fd, NSocketPtr pSocket) {
        std::lock_guard<std::mutex> lk(m_mapMutex);
        m_socketMap[m_newFd] = pSocket;
    }

    void delSocket(int32 fd) {
        std::lock_guard<std::mutex> lk(m_mapMutex);
        m_socketMap.erase(fd);
    }

private:
    std::shared_ptr<UTILS::StreamSocket> m_sSocket;
    int32 m_newFd;
    uint16 m_newPort;
    std::string m_newIp;
    UTILS::SchedulerPtr m_pS;
    UTILS::ReactorEpollPtr m_eReactor;
    std::mutex m_mapMutex;
    std::map<int32, NSocketPtr> m_socketMap;
};

#endif
