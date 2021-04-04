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

	{
		proto::server::MsgPing msg;
		msg.set_index(18989788);

		std::vector<char> data(msg.ByteSizeLong());
		CmdManager::msgSerialize(PROTOCPP::MsgPing, msg, &data[0]);
		log_debug("data[%lu]", msg.ByteSizeLong());


		proto::server::MsgPing seMsg;
		CmdManager::msgDeserialize(PROTOCPP::MsgPing, seMsg, &data[0], data.size());

		log_debug("msg index[%u]", seMsg.index());
	}

	{
		proto::server::map64 msg;
		for (int i = 0; i < 10; i++) {
			proto::server::pair64* pPair = msg.add_pair_list();
			pPair->set_key(i);
			pPair->set_value(i%5);
			log_info("Map key[%u] value[%u]", i , i%5);
		}
		std::string result = msg.SerializeAsString();
		log_info("result[%lu:%s]", result.size(), result.c_str());

		proto::server::map64 seMsg;
		seMsg.ParseFromString(result);
		for (const auto& pair : msg.pair_list()) {
			log_info("Map key[%u] value[%u]", pair.key(), pair.value());
		}
	}
}
#endif
