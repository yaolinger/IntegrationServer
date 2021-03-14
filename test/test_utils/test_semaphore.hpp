#ifndef TEST_UTILS_TEST_SEMAPHORE_HPP
#define TEST_UTILS_TEST_SEMAPHORE_HPP

#include <chrono>

#include "utils/log.hpp"
#include "utils/log_helper.hpp"
#include "utils/macros.hpp"
#include "utils/semaphore.hpp"
#include "utils/base_thread.hpp"

// 信号量测试
void TestSemaphore() {
    FUNC_TRACE;
    USING_NS_UTILS;
 
	Semaphore sem;
	auto thread1Func = [&sem]() {
		std::this_thread::sleep_for(std::chrono::seconds(2));
		sem.notify();
		log_info("Thread[%lu] run over.", std::this_thread::get_id());
	};

	auto thread2Func = [&sem] {
		sem.wait();
		log_info("Thread[%lu] start run.", std::this_thread::get_id());
	};

	BaseThread thread1(thread1Func);
	sem.wait();
	BaseThread thread2(thread2Func);
	log_warn("Notify thread2 run.");
	sem.notify();
	
	thread1.join();
	thread2.join();
}

#endif
