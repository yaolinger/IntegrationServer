#ifndef TEST_UTILS_TEST_REACTOR_TIMER_HPP
#define TEST_UTILS_TEST_REACTOR_TIMER_HPP

#include "utils/log.hpp"
#include "utils/log_helper.hpp"
#include "utils/macros.hpp"
#include "utils/rand.hpp"
#include "utils/reactor_epoll.hpp"
#include "utils/reactor_timer.hpp"
#include "utils/scheduler_leader_follower.hpp"
#include "utils/thread_pool.hpp"

// 定时器执行次数
#define TIMER_RUN_TIMES 10

// 定时器
std::shared_ptr<UTILS::ReactorTimer> g_timerPtr;
std::shared_ptr<UTILS::ReactorTimer> g_timerPtr2;
uint32 g_times = 0;

// 反应堆定时器测试
void TestReactorTimer() {
    FUNC_TRACE;
    USING_NS_UTILS;

    ReactorEpollPtr pRE = std::make_shared<ReactorEpoll>();
    std::shared_ptr<SchedulerLF> pS = std::make_shared<SchedulerLF>(SCHEDULE_LF_MODE_REACTOR|SCHEDULE_LF_MODE_TASK_LIST);
    if (!pS->init(pRE)) {
        log_error("SchedulerLF init failed.");
        return;
    }
    if (!pRE->init(pS)) {
        log_error("Reactor Epoll init failed.");
        return;
    }

    ThreadPool pool;
    pool.createThread([&pS](){ pS->run(); }, 5);
    pS->start();

    g_timerPtr = std::make_shared<ReactorTimer>(pRE);
    g_timerPtr2 = std::make_shared<ReactorTimer>(pRE);

    std::function<void()> func;
    func = [&]() {
        log_info("Do timer task times[%u] at thread[%lu]", ++g_times, std::this_thread::get_id());
        if (g_times < TIMER_RUN_TIMES) {
            // 随机添加批量任务
            {
                uint32 times = UTILS::Rand::randBetween((uint32)1, (uint32)10);
                for (uint32 i = 0; i < times; i++) {
                    g_timerPtr->expiresFunc(1, [](){ log_info("随机定时器1任务~"); });
                }
                if (g_times < (TIMER_RUN_TIMES/2)) {
                    for (uint32 i = 0; i < times; i++) {
                        g_timerPtr2->expiresFuncByMs(10, [](){ log_info("随机定时器2任务~"); });
                    }
                } else {
                    g_timerPtr2->cancelFunc();
                }
            }
            g_timerPtr->expiresFunc(1, func);
        } else {
            g_timerPtr->cancelFunc();
            pS->stop();
        }
    };
    UnitPtr pUnit = std::make_shared<SchdeulerUnit>(func);
    pS->post(pUnit);
}

#endif
