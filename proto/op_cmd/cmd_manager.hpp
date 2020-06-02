#ifndef PROTOCPP_CMD_MANAGER_HPP
#define PROTOCPP_CMD_MANAGER_HPP

#include <map>
#include <string>

#include "utils/macros.hpp"
#include "utils/log.hpp"

/* 文件修改位置
 * proto/op_cmd/cmd_manager.hpp
 */

NS_PROTOCPP_BEGIN

// 协议管理器
class CmdManager {
public:
	// 初始化
	static void init();
	// 清空协议
	static void clear();
	// 获取协议名
	static const char* getMsgDesc(uint32 cmd);

	// 协议序列化
	template <typename MSG_T>
	static void msgSerialize(uint16 cmd, const MSG_T& msg, char* pData);

	// 协议反序列化
	template <typename MSG_T>
	static void msgDeserialize(uint16 cmd, MSG_T& msg, const char* pData, uint16 size);

private:
	static std::map<uint32, std::string> m_descMap;
};

template <typename MSG_T>
void CmdManager::msgSerialize(uint16 cmd, const MSG_T& msg, char* pData) {
	if (!pData) {
		log_error("Null == pData, msg[%s:%u] serialize failed!", getMsgDesc(cmd), cmd);
		return;
	}
	if (!msg.SerializeToArray(pData, msg.ByteSizeLong())) {
		log_error("Msg[%s:%u] serialize failed!", getMsgDesc(cmd), cmd);
		return;
	}
}

template <typename MSG_T>
void CmdManager::msgDeserialize(uint16 cmd, MSG_T& msg, const char* pData, uint16 size) {
	if (!pData) {
		log_error("Null == pData, msg[%s:%d] deserialize failed", getMsgDesc(cmd), cmd);
		return;
	}
	if (!msg.ParseFromArray(pData, size)) {
		log_error("Msg[%s:%d] deserialize failed!", getMsgDesc(cmd), cmd);
		return;
	}
}

NS_PROTOCPP_END

// 获取协议名
#define CMD_DESC(cmd) \
    PROTOCPP::CmdManager::getMsgDesc(cmd)
// 消息序列化
#define MSG_SERIALIZE(cmd, msg, pData) \
    PROTOCPP::CmdManager::msgSerialize(cmd, msg, pData)
// 消息反序列化
#define MSG_DESERIALIZE(cmd, msg, pData, size) \
    PROTOCPP::CmdManager::msgDeserialize(cmd, msg, pData, size)

#endif
