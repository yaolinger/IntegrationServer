#ifndef BOOST_NETWORK_CONNECTOR_HPP
#define BOOST_NETWORK_CONNECTOR_HPP

#include <boost/asio/deadline_timer.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/io_service_strand.hpp>

#include "io_interface.hpp"
#include "utils/macros.hpp"
#include "utils/noncopyable.hpp"

NS_BOOST_NETWORK_BEGIN

class Connector : public IoInterface
				, public std::enable_shared_from_this<Connector>
			    , public utils::Noncopyable {
public:
	explicit Connector(boost::asio::io_service& ios, const std::string& ip, uint16 port, IoCallback* pIoCallback, uint32 reconnectTime);

public:
	void onReadError(const TcpSocketPtr& s, const boost::system::error_code& ec);

    void onWriteError(const TcpSocketPtr& s, const boost::system::error_code& ec);

	void onActiveSocketClose(const TcpSocketPtr& s);

    void onPassiveSocketClose(const TcpSocketPtr& s);

private:
	void connect();

	void reconnect();

public:
	// 停止主动链接
	void stop();
	// 清空链接
	void clear();

    const std::string& getIp() { return m_ip; }
	uint16 getPort() { return m_port; }

private:
	boost::asio::io_service::strand m_strand;
	boost::asio::ip::tcp::socket m_newSocket;

	TcpSocketPtr m_socketPtr;
	std::string m_ip;
	uint16 m_port;
	uint32 m_reconnectTime;
	bool m_startSign;                    // 开始标识
	boost::asio::deadline_timer m_timer; // 重连计时器
};

typedef std::shared_ptr<Connector> ConnectorPtr;

NS_BOOST_NETWORK_END

#endif
