#include "sentry_msg_handler.hpp"

#include "protocpp/op_cmd.hpp"
#include "protocpp/protos/server.pb.h"
#include "sentry_manager.hpp"

SERVICE_CORE::BoostMsgHandler SentryMsgHandler::s_msgHandler;

bool SentryMsgHandler::initMsgHandler() {
    bool ret = true;
    ret &= s_msgHandler.registerFunc(PROTOCPP::MsgRobot, handleMsgRobot);
    return ret;
}

bool SentryMsgHandler::handleMsgRobot(const BOOST_NETWORK::TcpSocketPtr& s, const BOOST_NETWORK::MsgBufPtr& buf) {
    auto pOb = ObjectManager::findObject(s->getBindUid());
    if (NULL == pOb) {
        log_error("NULL == pOb at socket[%d][%s:%u]", s->getSocketId(), s->getIp(), s->getPort());
        return false;
    }

    // 反序列化数据
    MSG_DESERIALIZE_BY_MSGBUF(PROTOCPP::MsgRobot, proto::server::MsgRobot, buf);
    log_info("Robot[%u] recv[%s]", pOb->getUid(), msg.test().c_str());

    return true;
}
