#ifndef SERVICE_CORE_BOOST_SERVER_HPP
#define SERVICE_CORE_BOOST_SERVER_HPP

#include "server_base.hpp"

#include <boost/asio/io_service.hpp>

NS_SERVICE_CORE_BEGIN

// boost服务
class BoostServer : public ServerBase {
public :
	BoostServer() {}
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
public:
	boost::asio::io_service& getIos() { return m_ios; }

private:
	boost::asio::io_service m_ios;
	std::shared_ptr<boost::asio::io_service::work> m_work;
};

NS_SERVICE_CORE_END

#endif
