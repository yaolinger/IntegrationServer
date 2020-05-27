#include "utils/log.hpp"
#include "utils/time_helper.hpp"
#include "test_utils/test_thread.hpp"
#include "test_utils/test_time.hpp"

void baseInit() {
    USING_NS_UTILS;
    TimeHelper::init();
    Logger::init();

    log_warn("~~~~~~~~~~~ TEST START ~~~~~~~~~~~~~~~~~~~");
}

int main() {
    baseInit();

	// test thread
	TestThread();

    // test timer;
    TestTimer();

    return 0;
}
