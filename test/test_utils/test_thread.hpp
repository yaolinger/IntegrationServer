#ifndef  TEST_UTILS_TEST_THREAD_HPP
#define  TEST_UTILS_TEST_THREAD_HPP

#include <iostream>
#include "utils/thread_pool.hpp"
#include "utils/time_base.hpp"
#include "utils/log.hpp"

void TestThread () {
	USING_NS_UTILS
	auto func = []() {
		std::this_thread::sleep_for(seconds(1));
	    log_trace("Thread[%ld] run thread func ~~~~~~~~~~~~~~~~~~", std::this_thread::get_id());
    };

	ThreadPool pool;
	pool.createThread(func, 4);
	pool.join();
}

#endif

