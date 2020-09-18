#ifndef TEST_UTISL_TEST_SCHEDULER_LF_HPP
#define TEST_UTISL_TEST_SCHEDULER_LF_HPP

#include <chrono>

#include "utils/log.hpp"
#include "utils/log_helper.hpp"
#include "utils/macros.hpp"
#include "utils/reactor.hpp"
#include "utils/scheduler_leader_follower.hpp"
#include "utils/scheduler_unit.hpp"
#include "utils/scheduler_safe_unit.hpp"
#include "utils/thread_pool.hpp"

void TestSchedulerLF() {
    FUNC_TRACE;
    USING_NS_UTILS;
    ReactorPtr pReactor = std::make_shared<FakeReactor>();
    SchedulerLF s(SCHEDULE_LF_MODE_REACTOR|SCHEDULE_LF_MODE_TASK_LIST);
    if (!s.init(pReactor)) {
        log_error("SchedulerLF init failed.");
        return;
    }

    ThreadPool pool;
    pool.createThread([&s](){ s.run(); }, 3);
    s.start();

    uint32 times = 10;
    pool.createThread([&times, &s]() {
                UnitPtr pUnit = std::make_shared<SchdeulerUnit>([&times, &s]() {
                            times--;
                            //log_debug("Do task times[%u]", times);
                            if (0 == times) {
                                s.stop();
                            }
                        });

                uint32 count = times;
                for (uint32 i = 0; i < count; i++) {
                    s.post(pUnit);
                    std::this_thread::sleep_for(std::chrono::milliseconds(500));
                }
            }, 1);

}

#endif
