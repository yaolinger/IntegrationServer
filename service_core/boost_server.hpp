#ifndef SERVICE_CORE_BOOST_SERVER_HPP
#define SERVICE_CORE_BOOST_SERVER_HPP

#include "server_base.hpp"

#include <boost/asio/io_service.hpp>
#include <boost/asio/signal_set.hpp>

#include "boost_network/network.hpp"

NS_SERVICE_CORE_BEGIN

// boost服务
class BoostServer : public ServerBase {
public :
	BoostServer() : m_ios(), m_singleWait(m_ios) {}
	~BoostServer() {}

// 待实现(逻辑层)
public:
    // 服务名称
    virtual const char* getServerName() = 0;
    // 初始化服务
    virtual bool onInitServer() = 0;
    // 服务开始运行
    virtual void onRunServer() = 0;
    // 关闭服务
    virtual bool onCloseServer() = 0;

public:
    // 初始化服务
    bool initServer();
    // 运行主体
    bool runServer();
    // 关闭服务
    bool closeServer();
    // 信号处理
    bool initSignal();
	// 服务打断处理
	void interruptedServer();

// 网络层
public:
    // 开启网络层
    bool startNetwork(uint32 threadNum);
    // 监听
    bool listenAt(const std::string& ip, uint16 port, boost_network::AcceptorCallback* pAcceptorCb);
    // 链接
    bool connectTo(const std::string& ip, uint16 port, boost_network::ConnectorCallback* pConnectorCb, uint32 reConnectTime);
    // 关闭网络层
    void closeNetwork();

public:
	boost::asio::io_service& getIos() { return m_ios; }

private:
	boost::asio::io_service m_ios;                           // boost 服务对象
    boost::asio::signal_set m_singleWait;                    // boost 信号捕捉对象(RAII机制 析构会强制调用handle)
	std::shared_ptr<boost::asio::io_service::work> m_work;   // boost 服务控制器

private:
    boost_network::BoostNetwork m_network;                   // 网络层
};

NS_SERVICE_CORE_END

#endif
