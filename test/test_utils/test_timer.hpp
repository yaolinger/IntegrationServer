#ifndef TEST_UTILS_TEST_TIMER_HPP
#define TEST_UTILS_TEST_TIMER_HPP

#include <boost/asio/io_service.hpp>

#include "utils/log.hpp"
#include "utils/log_helper.hpp"
#include "utils/macros.hpp"
#include "utils/time_helper.hpp"
#include "utils/time_manager.hpp"

void TestTimer() {
	USING_NS_UTILS;
	FUNC_TRACE;

	boost::asio::io_service ios;
	std::shared_ptr<boost::asio::io_service::work> work;
	TimerManager::init(ios);


	uint32 now = TimeHelper::getCurrentSecond();
	TimerPtr pTimer = std::make_shared<TimerBase>(1);
	std::function<void()> func = NULL;
	func = [&](){
		log_info("Timer func run, now[%u]", TimeHelper::getCurrentSecond());
		if (now + 5 < (uint32)TimeHelper::getCurrentSecond()) {
			work.reset();
			ios.stop();
		} else {
			pTimer->afterFunc(1, func);
		}

	};

	pTimer->afterFunc(1, func);

	work.reset(new boost::asio::io_service::work(ios));
	ios.run();
}

#endif
