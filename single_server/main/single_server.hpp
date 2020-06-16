#ifndef SINGLE_SERVER_MAIN_SINGLE_SERVER_HPP
#define SINGLE_SERVER_MAIN_SINGLE_SERVER_HPP

#include "protocpp/cmd_manager.hpp"
#include "sentry/sentry_listener.hpp"
#include "service_core/boost_server.hpp"
#include "service_core/boost_msg_handler.hpp"

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

    bool onInitLogic();

public:
	bool closeLogic();

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

public:
    template <typename T_MSG>
    static bool sendMsgBySocket(const BOOST_NETWORK::TcpSocketPtr& pSocket, uint16_t cmd, const T_MSG& msg);
};

template <typename T_MSG>
bool SingleServer::sendMsgBySocket(const BOOST_NETWORK::TcpSocketPtr& pSocket, uint16_t cmd, const T_MSG& msg){
    if( NULL == pSocket) {
        log_error("send cmd[%s:%d] to server failed: NULL == p_socket", CMD_DESC(cmd), cmd);
        return false;
    }

    MSG_SERIALIZE_TO_MSGBUF(cmd, msg);
    if( NULL == buf ){
        log_error("send socket[%d] cmd[%s:%d] to server failed: NULL == send_buf",  pSocket->getSocketId(), CMD_DESC(cmd), cmd);
        return false;
    }

    log_trace("Send messages cmd[%s:%d]", CMD_DESC(cmd), cmd);
    pSocket->write(cmd, buf);
    return true;
}
#endif
