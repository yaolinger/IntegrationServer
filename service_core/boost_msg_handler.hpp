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
    typedef std::function<bool(const BOOST_NETWORK::TcpSocketPtr& s, const BOOST_NETWORK::MsgBufPtr& buf)> MsgHandlerFunc;
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

        if (NULL == s) {
            log_error("NULL == s");
            return false;
        }

        if (NULL == buf) {
            log_error("NULL == buf");
            return false;
        }
        iter->second(s, buf);
        return true;
    }

private:
    std::map<uint16, MsgHandlerFunc> m_funcMap;
};

NS_SERVICE_CORE_END

// 基于MsgBuf序列化
#define MSG_SERIALIZE_TO_MSGBUF(cmd, msg) \
    BOOST_NETWORK::MsgBufPtr buf = std::make_shared<BOOST_NETWORK::MsgBuf>(msg.ByteSizeLong()); \
    MSG_SERIALIZE(cmd, msg, buf->data());

// 基于MsgBuf反序列化
#define MSG_DESERIALIZE_BY_MSGBUF(cmd, MSG_T, buf) \
    MSG_T msg; \
    MSG_DESERIALIZE(cmd, msg, buf->data(), buf->size());

#endif
