#include "network.hpp"

#include <boost/bind.hpp>

NS_BOOST_NETWORK_BEGIN

bool BoostNetwork::init(uint32 threadNum) {
	if (threadNum == 0) {
		log_error("Network threadNum must > 0, cur threadNum[%u]", threadNum);
		return false;
	}

	m_workPtr.reset(new boost::asio::io_service::work(m_ios));
	m_threadPool.createThread(boost::bind(&boost::asio::io_service::run, &m_ios), threadNum);
	return true;
}

void BoostNetwork::close() {
	// 清空监听
	clearListen();
	// 清空主动链接
	clearConnect();

	// 释放boost::work 结束io_service::run循环
	m_workPtr.reset();

	// 回收线程资源
	m_threadPool.join();

	// 清空监听链接
	m_listenMap.clear();
	// 清空主动链接
	m_connectVec.clear();
}

void BoostNetwork::stop() {
	// 停止监听
	stopListen();
	// 停止主动链接
	stopConnect();
}

bool BoostNetwork::listenAt(const std::string& ip, uint16 port, AcceptorCallback* pAcceptorCb) {
	if (m_listenMap.find(port) != m_listenMap.end()) {
		log_error("Port[%u] already listen.", port);
		return false;
	}

	if (NULL == pAcceptorCb) {
		log_error("NULL == pAcceptorCb.");
		return false;
	}

	// 建立acceptor对象开始监听
	AcceptorPtr pAcceptorPtr = std::make_shared<Acceptor>(m_ios, ip, port, pAcceptorCb);
	if (!pAcceptorPtr->listen()) {
		log_error("Listen at[%s:%u] failed!", ip.c_str(), port);
		return false;
	}
	m_listenMap.insert(std::make_pair(port, pAcceptorPtr));
	return true;
}

bool BoostNetwork::connectTo(const std::string& ip, uint16 port, ConnectorCallback* pConnectorCb, uint32 reConnectTime) {
	if (NULL == pConnectorCb) {
		log_error("NULL == pConnectorCb");
		return false;
	}

	// 建立Connect对象开始链接
	ConnectorPtr connectPtr = std::make_shared<Connector>(m_ios, ip, port, pConnectorCb, reConnectTime);
	m_connectVec.push_back(connectPtr);
	return true;
}

void BoostNetwork::postCloseSocket(const TcpSocketPtr& s) {
	if (NULL == s) {
		log_error("NULL == s");
		return;
	}

	IoInterface* pIoInterface = s->getIoInterface();
	if (NULL == pIoInterface) {
		log_error("NULL == pIoInterface at socket[%d][$s:%u]", s->getSocketId(), s->getIp().c_str(), s->getPort());
		return;
	}

	pIoInterface->onDelSocket(s);
}

void BoostNetwork::stopListen() {
	for (const auto& iter : m_listenMap) {
		if (iter.second) {
			iter.second->stop();
		}
	}
}

void BoostNetwork::clearListen() {
	for (const auto& iter : m_listenMap) {
		if (iter.second) {
			iter.second->clear();
		}
	}
}

void BoostNetwork::stopConnect() {
	for (const auto& iter : m_connectVec) {
		if (iter) {
			iter->stop();
		}
	}
}

void BoostNetwork::clearConnect() {
	for (const auto& iter : m_connectVec) {
		if (iter) {
			iter->clear();
		}
	}
}

NS_BOOST_NETWORK_END
