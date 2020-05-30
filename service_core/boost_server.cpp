#include "boost_server.hpp"

#include <signal.h>

#include <boost/asio/signal_set.hpp>
#include <boost/bind.hpp>
#include <google/protobuf/io/coded_stream.h>
#include <google/protobuf/io/zero_copy_stream_impl.h>

#include "utils/log.hpp"
#include "utils/time_helper.hpp"

NS_SERVICE_CORE_BEGIN
USING_NS_UTILS

static void handleSignal(const boost::system::error_code& ec, int32 signalNumber) {
	if (ec) {
	 	log_error("signal[%d] error: %s", signalNumber, ec.message().c_str());
		return;
	}

	log_info("recv terminate signal: %d", signalNumber);

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
	struct sigaction sig_ing;
	sig_ing.sa_handler = SIG_IGN;
	sigaction(SIGHUP, &sig_ing, NULL);
	sigaction(SIGPIPE, &sig_ing, NULL);

	boost::asio::signal_set sig_wait(m_ios);
	sig_wait.add(SIGINT);
	sig_wait.add(SIGTERM);
	sig_wait.add(SIGQUIT);
	sig_wait.async_wait(boost::bind(handleSignal, _1, _2));
	return true;
}

void BoostServer::interruptedServer() {
	m_ios.post([this]() {
				closeServer();
			});
}

NS_SERVICE_CORE_END
