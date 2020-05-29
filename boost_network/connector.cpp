#include "connector.hpp"

#include <boost/asio/ip/address.hpp>

#include "tcp_socket.hpp"
#include "utils/log.hpp"

NS_BOOST_NETWORK_BEGIN
USING_NS_UTILS

Connector::Connector(boost::asio::io_service& ios, const std::string& ip, uint16 port, IoCallback* pIoCallback, uint32 reconnectTime)
	: IoInterface(ios, pIoCallback)
	, m_strand(ios)
	, m_newSocket(ios)
	, m_socketPtr(NULL)
	, m_ip(ip)
	, m_port(port)
	, m_reconnectTime(reconnectTime)
	, m_startSign(false)
	, m_timer(ios) {
	connect();
}

void Connector::onReadError(const TcpSocketPtr& s, const boost::system::error_code& ec) {
	if (!s) {
		log_error("NULL == s");
		return;
	}
	log_error("[%s] disconnect to socket[%u][%s:%u], error[%d:%s]", m_ioCallback->name(), m_socketPtr->getSocketId(), m_ip.c_str(), m_port, ec.value(), ec.message().c_str());
}

void Connector::onWriteError(const TcpSocketPtr& s, const boost::system::error_code& ec) {
	if (!s) {
		log_error("NULL == s");
		return;
	}
	log_error("[%s] disconnect to socket[%u][%s:%u], error[%d:%s]", m_ioCallback->name(), m_socketPtr->getSocketId(), m_ip.c_str(), m_port, ec.value(), ec.message().c_str());
}

void Connector::onDelSocket(const TcpSocketPtr& s) {
	if (!s) {
		log_error("NULL == s");
		return;
	}
	m_strand.post([this, s]() {
				if (!m_socketPtr) {
					return;
				}

				if (s != m_socketPtr) {
					log_error("s != m_socketPtr");
					return;
				}

				m_socketPtr->close();
				m_socketPtr.reset();

				reconnect();
			});
}

void Connector::onSocketClose(const TcpSocketPtr& s) {
	if (!s) {
		log_error("NULL == s");
		return;
	}
	m_strand.post([this, s]() {
				if (!m_socketPtr) {
					return;
				}

				if (s != m_socketPtr) {
					log_error("s != m_socketPtr");
					return;
				}

				m_ioCallback->onClose(s);
				m_socketPtr.reset();

				reconnect();
			});
}

void Connector::connect() {
	boost::asio::ip::tcp::endpoint ep(boost::asio::ip::address::from_string(m_ip), m_port);
	m_newSocket.async_connect(ep, m_strand.wrap([this, ep](const boost::system::error_code& ec) {
					if (ec) {
					    log_error("[%s] connect to socket[%u][%s:%u] failed, error[%d, %s]", m_ioCallback->name(), m_newSocket.native_handle(), ep.address().to_string().c_str(), ep.port(), ec.value(), ec.message().c_str());

						boost::system::error_code ignored_ec;
						m_newSocket.shutdown(boost::asio::ip::tcp::socket::shutdown_both, ignored_ec);
						m_newSocket.close(ignored_ec);

						if (ec == boost::asio::error::operation_aborted) {
							return;
						}
						reconnect();
					} else {
						log_trace("[%s] connect to socket[%u][%s:%u] success.", m_ioCallback->name(), m_newSocket.native_handle(), ep.address().to_string().c_str());
						m_socketPtr.reset(new TcpSocket(m_ios, std::move(m_newSocket), ep, this));
						m_ioCallback->onConnect(m_socketPtr);
						m_socketPtr->read();
					}
				}));
}

void Connector::reconnect() {
	if (m_reconnectTime == 0) {
		return;
	}

	// stop 后取消重连
	if (!m_startSign) {
		return;
	}

	m_timer.expires_from_now(boost::posix_time::seconds(m_reconnectTime));
	m_timer.async_wait(m_strand.wrap([this](const boost::system::error_code& ec) {
					if (ec == boost::asio::error::operation_aborted) {
						return;
					} else if (ec) {
						log_error("Timer error: %s", ec.message().c_str());
						return;
					}

					connect();

					log_trace("[%s] start reconnect to [%s:%u]", m_ioCallback->name(), m_ip.c_str(), m_port);
				}));

}

void Connector::stop() {
	m_strand.post([this]() {
				m_startSign = false;

				// 建立链接失败 执行取消socket操作
				// 此时挂在socket上的操作为reconnect connect
				if (!m_socketPtr) {
					boost::system::error_code ec;
					m_newSocket.cancel(ec);
					CHECK_ERROR_LOG(ec, "[%s] socket cancel error[%d:%s]", m_ioCallback->name(), ec.value(), ec.message().c_str());
				}

				// 取消定时器
				m_timer.cancel();
			});
}

void Connector::clear() {
	m_strand.post([this]() {
				// 建立链接成功 执行socket关闭逻辑
				if (m_socketPtr) {
					m_socketPtr->close();
					m_socketPtr.reset();
				}
			});
}

NS_BOOST_NETWORK_END
