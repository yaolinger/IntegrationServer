#ifndef TEST_UTILS_TEST_SIGNAL_CONTROL_HPP
#define TEST_UTILS_TEST_SIGNAL_CONTROL_HPP

#include <signal.h>

#include "utils/log.hpp"
#include "utils/log_helper.hpp"
#include "utils/macros.hpp"
#include "utils/reactor_fake.hpp"
#include "utils/scheduler_leader_follower.hpp"
#include "utils/scheduler_unit.hpp"
#include "utils/signal_control.hpp"

// 信号捕捉测试
void TestSignalControl() {
    FUNC_TRACE;
    USING_NS_UTILS;
    ReactorPtr pReactor = std::make_shared<ReactorFake>();
	std::shared_ptr<SchedulerLF> pScheduler = std::make_shared<SchedulerLF>(SCHEDULE_LF_MODE_REACTOR|SCHEDULE_LF_MODE_TASK_LIST);
    if (!pScheduler->init(pReactor)) {
        log_error("SchedulerLF init failed.");
        return;
    }

    ThreadPool pool;
    pool.createThread([&pScheduler](){ pScheduler->run(); }, 3);
    pScheduler->start();

	// 信号处理	
	if (!SignalControl::init(pScheduler)) {
		log_error("SignalControl init failed.");
		return;
	}
	SignalControl::addSignal(SIGINT);
	SignalControl::asyncWait([&](int32 signal){
		log_warn("Recv signal [%d] stop process", signal);
		SignalControl::reset();
		pScheduler->stop();
	});
}

#endif

