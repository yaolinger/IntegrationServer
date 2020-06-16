#include "utils/log.hpp"
#include "utils/time_helper.hpp"
#include "test_utils/test_thread.hpp"
#include "test_utils/test_time.hpp"
#include "test_utils/test_timer.hpp"
#include "test_protocpp/test_msg.hpp"

void baseInit() {
    USING_NS_UTILS;
    TimeHelper::init();
    Logger::init();

    log_warn("~~~~~~~~~~~ TEST START ~~~~~~~~~~~~~~~~~~~");
}

int main() {
    baseInit();

	// test thread
	// TestThread();

    // test time
    // TestTime();

	// test timer
	TestTimer();

	// test msg
	// TestMsg();

    return 0;
}
