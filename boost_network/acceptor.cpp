#include "acceptor.hpp"

#include "tcp_socket.hpp"
#include "utils/log.hpp"

NS_BOOST_NETWORK_BEGIN
USING_NS_UTILS

Acceptor::Acceptor(boost::asio::io_service& ios, const std::string& listenIp, uint16 port, IoCallback* pIoCallback)
    : IoInterface(ios, pIoCallback)
    , m_strand(ios)
    , m_acceptor(ios)
    , m_newSocket(ios)
    , m_listenIp(listenIp)
    , m_listenPort(port) {
}

void Acceptor::onReadError(const TcpSocketPtr& s, const boost::system::error_code& ec) {
    if (!s) {
        log_error("NULL == s");
        return;
    };
    log_warn("%s[%s:%d] was disconnected, error[%d:%s]", m_ioCallback->name(), s->getIp().c_str(), s->getSocketId(), ec.value(), ec.message().c_str());
}

void Acceptor::onWriteError(const TcpSocketPtr& s, const boost::system::error_code& ec) {
    if (!s) {
        log_error("NULL == s");
        return;
    };
    log_warn("%s[%s:%d] was disconnected, error[%d:%s]", m_ioCallback->name(), s->getIp().c_str(), s->getSocketId(), ec.value(), ec.message().c_str());
}

void Acceptor::onSocketClose(const TcpSocketPtr& s) {
    if (!s) {
        log_error("NULL == s");
        return;
    }

    m_strand.post([this, s]() {
                auto iter = std::find_if(m_socketVec.begin(), m_socketVec.end(), [s](const TcpSocketPtr& pSocketPtr) {
                            if (s == pSocketPtr) {
                                return true;
                            }
                            return false;
                        });
                if (iter == m_socketVec.end()) {
                    return;
                }

                m_ioCallback->onClose(s);
                m_socketVec.erase(iter);
            });
}

bool Acceptor::listen() {
    try {
        boost::asio::ip::tcp::resolver resolver(m_ios);
        boost::asio::ip::tcp::endpoint ep = *resolver.resolve(m_listenIp, std::to_string(m_listenPort)).begin();
        m_acceptor.open(ep.protocol());
        m_acceptor.set_option(boost::asio::ip::tcp::acceptor::reuse_address(true));
        m_acceptor.bind(ep);
        m_acceptor.listen();
        accept();

        log_trace("[%s] listen at[%s:%u] success.", m_ioCallback->name(), ep.address().to_string().c_str(), ep.port());
    } catch (boost::system::system_error& se) {
        log_error("[%s] listen at[%s:%u] error:%s.", m_ioCallback->name(), m_listenIp.c_str(), m_listenPort);
        return false;
    }

    return true;
}

void Acceptor::stop() {
    m_strand.post([this]() {
                if (!m_acceptor.is_open()) {
                    return;
                }

                log_trace("[%s] listen[%s:%u] close!", m_ioCallback->name(), m_listenIp.c_str(), m_listenPort);
                boost::system::error_code ec;
                m_acceptor.close(ec);

                CHECK_ERROR_LOG(ec, "[%s] listen at [%s:%u] close error[%u:%s]", m_ioCallback->name(), m_listenIp.c_str(), m_listenPort, ec.value(), ec.message().c_str());
            });
}

void Acceptor::clear() {
    m_strand.post([this]() {
                for (auto& iter : m_socketVec) {
                    if (!iter) {
                        continue;
                    }
                    iter->close();
                }

                m_socketVec.clear();
            });
}

void Acceptor::accept() {
    m_acceptor.async_accept(m_newSocket, m_newEndpoint, m_strand.wrap([this](const boost::system::error_code& ec) {
                    if (ec == boost::asio::error::operation_aborted) {
                        return;
                    }

                    if (!m_acceptor.is_open()) {
                        return;
                    }

                    do {
                        // accept 异常不处理
                        if (ec) {
                            break;
                        }

                        log_debug("[%s] new scoket[%d][%s:%u] connect.", m_ioCallback->name(), m_newSocket.native_handle(), m_newEndpoint.address().to_string(), m_newEndpoint.port());

                        TcpSocketPtr pSocketPtr = std::make_shared<TcpSocket>(m_ios, std::move(m_newSocket), m_newEndpoint, this);
                        m_socketVec.push_back(pSocketPtr);
                        m_ioCallback->onNewAccept(pSocketPtr);
                        pSocketPtr->read();
                    } while(0);

                    accept();
                }));
}

NS_BOOST_NETWORK_END
