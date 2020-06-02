#ifndef SINGLE_SERVER_MAIN_SINGLE_SERVER_HPP
#define SINGLE_SERVER_MAIN_SINGLE_SERVER_HPP

#include "sentry/sentry_listener.hpp"
#include "service_core/boost_server.hpp"

class SingleServer : public SERVICE_CORE::BoostServer {
public:
    SingleServer() {}
    ~SingleServer() {}

public:
    const char* getServerName() { return "SingleServer"; }

    bool onInitServer();

    void onRunServer();

    bool onCloseServer();

    bool onInitNetwork();

    // 网络层回调
public:
    void onAddSentry(const BOOST_NETWORK::TcpSocketPtr& s);
    void onDelSentry(const BOOST_NETWORK::TcpSocketPtr& s);
    void onSentryMsg(const BOOST_NETWORK::TcpSocketPtr& s, uint16 cmd, const BOOST_NETWORK::MsgBufPtr& buf);

private:
    // 读取配置
    bool onLoadConfig();
    // 读取xml
    bool onLoadXmlConfig();

private:
    SentryListener m_sentryListner;   // 监听
};
#endif
