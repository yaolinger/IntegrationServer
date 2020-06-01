#include "boost_server.hpp"

#include <signal.h>

#include <boost/bind.hpp>
#include <google/protobuf/io/coded_stream.h>
#include <google/protobuf/io/zero_copy_stream_impl.h>

#include "utils/log.hpp"
#include "utils/time_helper.hpp"

NS_SERVICE_CORE_BEGIN
USING_NS_UTILS
USING_NS_BOOST_NETWORK

static void handleSignal(const boost::system::error_code& ec, int32 signalNumber) {
	log_warn("Recv terminate signal: %d, error:[%d:%s]", signalNumber, ec.value(), ec.message().c_str());

	ServerBase* pInstance = ServerBase::getInstance();
	if (NULL != pInstance && !pInstance->istInterrupted()) {
		pInstance->setInterrupted();
		pInstance->interruptedServer();
	}
}

bool BoostServer::initServer() {
	StopWatch sw;
	// 时间初始化
	TimeHelper::init();
	// 日志初始化
	Logger::init();

	if (!onInitServer()) {
		return false;
	}

	log_trace("[%s] init success, spend time[%u](ms)~~", getServerName(), sw.durationMs());
	return true;
}

bool BoostServer::runServer() {
	if (!initSignal()) {
		return false;
	}

	if (!initServer()) {
		return false;
	}

	onRunServer();

	m_work.reset(new boost::asio::io_service::work(m_ios));
	m_ios.run();
	return true;
}

bool BoostServer::closeServer() {
	log_trace("[%s] server stoping.....", getServerName());

	if (!onCloseServer()) {
		return false;
	}

	m_work.reset();
	m_ios.stop();

	// protobuf 关服释放内存
	google::protobuf::ShutdownProtobufLibrary();
	return true;
}

bool BoostServer::initSignal() {
	// 忽略信号 SIGHUB SIGPIPE
    struct sigaction siging;
	siging.sa_handler = SIG_IGN;
	sigaction(SIGHUP, &siging, NULL);
	sigaction(SIGPIPE, &siging, NULL);

    // 捕捉信号 SIGINT SIGTREAM SIGQUIT
	try {
        m_singleWait.add(SIGINT);
	    m_singleWait.add(SIGTERM);
	    m_singleWait.add(SIGQUIT);
    } catch(boost::system::error_code ec) {
        log_error("Signal add error[%d:%s]", ec.value(), ec.message().c_str());
        return false;
    }
	m_singleWait.async_wait(boost::bind(handleSignal, _1, _2));
	return true;
}

void BoostServer::interruptedServer() {
	m_ios.post([this]() {
				closeServer();
			});
}

bool BoostServer::startNetwork(uint32 threadNum) {
	return m_network.init(threadNum);
}

bool BoostServer::listenAt(const std::string& ip, uint16 port, AcceptorCallback* pAcceptorCb) {
	return m_network.listenAt(ip, port, pAcceptorCb);
}

bool BoostServer::connectTo(const std::string& ip, uint16 port, ConnectorCallback* pConnectorCb, uint32 reConnectTime) {
	return m_network.connectTo(ip, port, pConnectorCb, reConnectTime);
}

void BoostServer::closeNetwork() {
	m_network.stop();
	m_network.close();
}

NS_SERVICE_CORE_END
