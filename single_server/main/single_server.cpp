#include "single_server.hpp"

#include "utils/time_helper.hpp"
#include "utils/log.hpp"

USING_NS_UTILS

bool SingleServer::onInitServer() {
	return true;
}

void SingleServer::onRunServer() {
	log_info("[%s start at %s]", getServerName(), TimeHelper::getNowTimeStr().c_str());
	setbuf(stdout, NULL);
}

bool SingleServer::onCloseServer() {
	return true;
}
