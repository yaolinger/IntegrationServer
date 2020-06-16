#ifndef SINGLE_SERVER_SENTRY_MSG_HANDLER_HPP
#define SINGLE_SERVER_SENTRY_MSG_HANDLER_HPP

#include "protocpp/cmd_manager.hpp"
#include "service_core/boost_msg_handler.hpp"

class SentryMsgHandler {
public:
    static bool initMsgHandler();

    static bool handleMsgRobot(const BOOST_NETWORK::TcpSocketPtr& s, const BOOST_NETWORK::MsgBufPtr& buf);

	static bool handleMsgPong(const BOOST_NETWORK::TcpSocketPtr& s, const BOOST_NETWORK::MsgBufPtr& buf);
public:
    static SERVICE_CORE::BoostMsgHandler s_msgHandler;
};

#endif
