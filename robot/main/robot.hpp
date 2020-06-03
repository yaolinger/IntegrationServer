#ifndef ROBOT_ROBOT_HPP
#define ROBOT_ROBOT_HPP

#include "boost_network/message.hpp"
#include "logic/logic_connector.hpp"
#include "protocpp/cmd_manager.hpp"
#include "service_core/boost_server.hpp"
#include "service_core/boost_msg_handler.hpp"

class Robot : public SERVICE_CORE::BoostServer {
public:
    Robot() {}
    ~Robot() {}

public:
    const char* getServerName() { return "Robot"; }

    bool onInitServer();

    void onRunServer();

    bool onCloseServer();

    bool onInitNetwork();

    bool onInitLogic();

public:
    void onLogicConnect(const BOOST_NETWORK::TcpSocketPtr& s);

    void onLogicMsg(const BOOST_NETWORK::TcpSocketPtr& s, uint16 cmd, const BOOST_NETWORK::MsgBufPtr& buf);

    void onLogicClose(const BOOST_NETWORK::TcpSocketPtr& s);

private:
    // 读取配置
    bool onLoadConfig();
    // 读取xml
    bool onLoadXmlConfig();

private:
    LogicConnector m_connector;

public:
    template <typename T_MSG>
    static bool sendMsgBySocket(const BOOST_NETWORK::TcpSocketPtr& pSocket, uint16_t cmd, const T_MSG& msg);
};

template <typename T_MSG>
bool Robot::sendMsgBySocket(const BOOST_NETWORK::TcpSocketPtr& pSocket, uint16_t cmd, const T_MSG& msg){
    if( NULL == pSocket) {
        log_error("send cmd[%s:%d] to server failed: NULL == p_socket", CMD_DESC(cmd), cmd);
        return false;
    }

    MSG_SERIALIZE_TO_MSGBUF(cmd, msg);
    if( NULL == buf ){
        log_error("send socket[%d] cmd[%s:%d] to server failed: NULL == send_buf",  pSocket->getSocketId(), CMD_DESC(cmd), cmd);
        return false;
    }
    pSocket->write(cmd, buf);
    return true;
}

#endif
