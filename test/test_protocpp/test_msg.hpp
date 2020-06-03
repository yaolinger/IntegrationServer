#ifndef TEST_PROTOCPP_TEST_MSG_HPP
#define TEST_PROTOCPP_TEST_MSG_HPP

#include "protocpp/cmd_manager.hpp"
#include "protocpp/op_cmd.hpp"
#include "protocpp/protos/server.pb.h"
#include "utils/log_helper.hpp"
#include "utils/macros.hpp"

void TestMsg() {
    FUNC_TRACE;
	USING_NS_PROTOCPP;
	CmdManager::init();

	proto::server::MsgPing msg;
	msg.set_index(1);

	std::vector<char> data(msg.ByteSizeLong());
	CmdManager::msgSerialize(PROTOCPP::MsgPing, msg, &data[0]);

	proto::server::MsgPing seMsg;
	CmdManager::msgDeserialize(PROTOCPP::MsgPing, seMsg, &data[0], data.size());

	log_debug("msg index[%u]", seMsg.index());
}
#endif
