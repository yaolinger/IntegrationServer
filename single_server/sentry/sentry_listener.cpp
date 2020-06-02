#include "sentry_listener.hpp"

#include "main/server_global.hpp"
#include "main/single_server.hpp"
#include "utils/macros.hpp"
#include "utils/log.hpp"

USING_NS_BOOST_NETWORK

// 网络层 -> 逻辑层

void SentryListener::onNewAccept(const TcpSocketPtr& s) {
    global::pServer->getIos().post(std::bind(&SingleServer::onAddSentry, global::pServer, s));
}

void SentryListener::onNewMsg(const TcpSocketPtr& s, uint16 cmd, const MsgBufPtr& buf) {
    global::pServer->getIos().post(std::bind(&SingleServer::onSentryMsg, global::pServer, s, cmd, buf));
}

void SentryListener::onClose(const TcpSocketPtr& s) {
    global::pServer->getIos().post(std::bind(&SingleServer::onDelSentry, global::pServer, s));
}
