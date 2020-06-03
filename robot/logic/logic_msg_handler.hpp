#ifndef ROBOT_LOGIC_MSG_HANDLER_HPP
#define ROBOT_LOGIC_MSG_HANDLER_HPP

#include "protocpp/cmd_manager.hpp"
#include "service_core/boost_msg_handler.hpp"

class LogicMsgHandler {
public:
    static bool initMsgHandler();


public:
    static SERVICE_CORE::BoostMsgHandler s_msgHandler;
};

#endif
