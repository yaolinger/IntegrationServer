#include "logic_connector.hpp"

#include "main/robot_global.hpp"
#include "main/robot.hpp"
#include "utils/macros.hpp"
#include "utils/log.hpp"

USING_NS_BOOST_NETWORK

// 网络层 -> 逻辑层

void LogicConnector::onConnect(const TcpSocketPtr& s) {
    global::pRobot->getIos().post(std::bind(&Robot::onLogicConnect, global::pRobot, s));
}

void LogicConnector::onNewMsg(const TcpSocketPtr& s, uint16 cmd, const MsgBufPtr& buf) {
    global::pRobot->getIos().post(std::bind(&Robot::onLogicMsg, global::pRobot, s, cmd, buf));
}

void LogicConnector::onClose(const TcpSocketPtr& s) {
    global::pRobot->getIos().post(std::bind(&Robot::onLogicClose, global::pRobot, s));
}
