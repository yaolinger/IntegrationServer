#include "logic_msg_handler.hpp"

#include "main/robot.hpp"
#include "main/robot_global.hpp"
#include "protocpp/op_cmd.hpp"
#include "protocpp/protos/server.pb.h"

SERVICE_CORE::BoostMsgHandler LogicMsgHandler::s_msgHandler;

bool LogicMsgHandler::initMsgHandler() {
    bool ret = true;
    ret &= s_msgHandler.registerFunc(PROTOCPP::MsgPing, handleMsgPing);
    return true;
}

bool LogicMsgHandler::handleMsgPing(const BOOST_NETWORK::TcpSocketPtr& s, const BOOST_NETWORK::MsgBufPtr& buf) {
    // 反序列化数据
    MSG_DESERIALIZE_BY_MSGBUF(PROTOCPP::MsgPing, proto::server::MsgPing, buf);

    proto::server::MsgPong reply;
    global::pRobot->sendMsgBySocket(s, PROTOCPP::MsgPong, reply);
    return true;
}
