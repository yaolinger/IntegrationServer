#include "single_server.hpp"

#include <boost/foreach.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>

#include "sentry/sentry_manager.hpp"
#include "sentry/sentry_msg_handler.hpp"
#include "utils/time_helper.hpp"
#include "utils/log.hpp"

USING_NS_UTILS
USING_NS_BOOST_NETWORK

bool SingleServer::onInitServer() {
	if (!onLoadConfig()) {
        log_error("Load config failed.");
        return false;
    }

    if (!onInitNetwork()) {
        log_error("Init network failed.");
        return false;
    }
    return true;
}

void SingleServer::onRunServer() {
	log_info("[%s start at %s]", getServerName(), TimeHelper::getNowTimeStr().c_str());
	setbuf(stdout, NULL);
}

bool SingleServer::onCloseServer() {
    closeNetwork();
    return true;
}

bool SingleServer::onLoadConfig() {
    if (!onLoadXmlConfig()) {
        return false;
    }
    return true;
}

bool SingleServer::onInitNetwork() {
    // TODO 暂时固化配置
    startNetwork(1);

    // 监听
    if (!listenAt("127.0.0.1", 23333, &m_sentryListner)) {
        return false;
    }

    // init msg
    SentryMsgHandler::initMsgHandler();
    return true;
}

bool SingleServer::onLoadXmlConfig() {
    return true;
}

void SingleServer::onAddSentry(const TcpSocketPtr& s) {
    ObjectManager::addConnector(s);
}

void SingleServer::onDelSentry(const TcpSocketPtr& s) {
    ObjectManager::delConnector(s);
}

void SingleServer::onSentryMsg(const TcpSocketPtr& s, uint16 cmd, const MsgBufPtr& buf) {
    SentryMsgHandler::s_msgHandler.msgHandler(s, cmd, buf);
}

