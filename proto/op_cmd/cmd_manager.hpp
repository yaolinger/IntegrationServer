#ifndef PROTOCPP_CMD_MANAGER_HPP
#define PROTOCPP_CMD_MANAGER_HPP

#include <map>
#include <string>

#include "utils/macros.hpp"

NS_PROTOCPP_BEGIN

// 协议管理器
class CmdManager {
public:
	static void init();

	static void clear();

	static const std::string getMsgDesc(uint32 cmd);

private:
	static std::map<uint32, std::string> m_descMap;
};

NS_PROTOCPP_END

#endif
