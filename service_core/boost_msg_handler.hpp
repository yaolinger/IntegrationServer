#ifndef SERVICE_CORE_BOOST_MSG_HANDLER_HPP
#define SERVICE_CORE_BOOST_MSG_HANDLER_HPP

#include <functional>
#include <map>

#include "boost_network/message.hpp"
#include "boost_network/tcp_socket.hpp"
#include "protocpp/cmd_manager.hpp"
#include "utils/log.hpp"
#include "utils/macros.hpp"

NS_SERVICE_CORE_BEGIN

// 消息回调
// 基于boost network
class BoostMsgHandler {
public:
    typedef std::function<bool(const BOOST_NETWORK::TcpSocketPtr& s,  const BOOST_NETWORK::MsgBufPtr& buf)> MsgHandlerFunc;
public:
    // 协议注册
    bool registerFunc(uint16 cmd, MsgHandlerFunc func) {
        if (NULL == func) {
            log_error("NULL == func for cmd[%d:%s].", cmd, CMD_DESC(cmd));
            return false;
        }

        if (m_funcMap.find(cmd) != m_funcMap.end()) {
            log_error("Duplicate register func for cmd[%d:%s].", cmd, CMD_DESC(cmd));
            return false;
        }

        m_funcMap[cmd] = func;
        return true;
    }

    // 消息处理
    bool msgHandler(const BOOST_NETWORK::TcpSocketPtr& s, uint16 cmd, const BOOST_NETWORK::MsgBufPtr& buf) {
        auto iter = m_funcMap.find(cmd);
        if (iter == m_funcMap.end() || NULL == iter->second) {
            log_error("Cmd[%d:%s] can not find func", cmd, CMD_DESC(cmd));
            return false;
        }
        iter->second(s, buf);
        return true;
    }

private:
    std::map<uint16, MsgHandlerFunc> m_funcMap;
};

NS_SERVICE_CORE_END

#endif
