#ifndef BOOST_NETWORK_HPP
#define BOOST_NETWORK_HPP

#include <map>
#include <vector>

#include "acceptor.hpp"
#include "connector.hpp"
#include "io_interface.hpp"
#include "tcp_socket.hpp"
#include "utils/macros.hpp"
#include "utils/thread_pool.hpp"

NS_BOOST_NETWORK_BEGIN

// 网络对象
class BoostNetwork {
public:
    BoostNetwork() {}
	~BoostNetwork() {}

public:
	// 初始化
	bool init(uint32 threadNum);

	// 关闭网络服务
	void close();

	// 停止网络服务
	void stop();

	// 监听
	bool listenAt(const std::string& ip, uint16 port, AcceptorCallback* pAcceptorCb);

	// 主动链接
	bool connectTo(const std::string& ip, uint16 port, ConnectorCallback* pConnectorCb, uint32 reConnectTime);

	// 主动关闭socket
	void postCloseSocket(const TcpSocketPtr& s);

protected:
	// 停止监听
	void stopListen();
	// 清空监听
	void clearListen();

	// 停止主动链接
	void stopConnect();
	// 清空主动链接
	void clearConnect();

private:
	boost::asio::io_service m_ios;                                // boost asio 服务模块
	std::shared_ptr<boost::asio::io_service::work> m_workPtr;     // 服务模块控制
	UTILS::ThreadPool m_threadPool;                               // 线程池

	std::map<uint16, AcceptorPtr> m_listenMap;  // 监听map(port:acceptor)
	std::vector<ConnectorPtr> m_connectVec;     // 主动链接vec
};

NS_BOOST_NETWORK_END

#endif
