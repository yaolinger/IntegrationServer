#ifndef TEST_SYNTAX_TEST_MUTEX
#define TEST_SYNTAX_TEST_MUTEX

#include <mutex>

#include "utils/base_thread.hpp"
#include "utils/log.hpp"
#include "utils/log_helper.hpp"
#include "utils/macros.hpp"

// <mutex>测试

void TestMutex() {
    FUNC_TRACE;
    USING_NS_UTILS;

    {
        log_warn("****************std::call_once***************");
        std::once_flag flag1;
        uint32 times = 0;
        auto func = [&times]() {
            log_info("Func run times[%u]", ++times);
        };

        std::call_once(flag1, func);
        std::call_once(flag1, func);
        std::call_once(flag1, func);
    } 

	{
		log_warn("**********std::mutex 多线程加解锁测试 ***************");
		std::mutex testMutex;
	    BaseThread lockThread([&]() {
			while(true) {
				log_info("尝试加锁");			
				testMutex.lock();
				log_info("加锁成功[%lu]", std::this_thread::get_id());			
			}
		});
		std::this_thread::sleep_for(std::chrono::seconds(3));
		BaseThread unlockThread([&](){
			while(true) {
				log_info("尝试解锁");
				testMutex.unlock();
				log_info("解锁成功[%lu]", std::this_thread::get_id());			
				std::this_thread::sleep_for(std::chrono::seconds(1));
			}
		});
		lockThread.join();
		unlockThread.join();
	}

}

#endif
