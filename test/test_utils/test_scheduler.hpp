#ifndef TEST_UTISL_TEST_SCHEDULER_HPP
#define TEST_UTISL_TEST_SCHEDULER_HPP

#include <chrono>
#include <functional>

#include "utils/log.hpp"
#include "utils/log_helper.hpp"
#include "utils/macros.hpp"
#include "utils/scheduler.hpp"
#include "utils/scheduler_unit.hpp"
#include "utils/scheduler_safe_unit.hpp"
#include "utils/thread_pool.hpp"

void TestScheduler() {
    FUNC_TRACE
    USING_NS_UTILS
    Scheduler s(SCHEDULER_MODE_CONSUMER);
    ThreadPool pool;
    pool.createThread([&s](){ s.workThreadFunc(); }, 3);
    s.start();

    uint32 unSafetimes = 10;
    uint32 times = 10;
    pool.createThread([&s, &unSafetimes](){
               UnitPtr pUnit = std::make_shared<SchdeulerUnit>([&unSafetimes, &s]() {
                       --unSafetimes;
                       if (unSafetimes == 0) {
                           log_debug("UnSafe unit test over;");
                       }
                    });
               uint32 count = unSafetimes;
               for (uint32 i = 0; i< count; i++) {
                   s.post(pUnit);
               }
            },1);

    pool.createThread([&s, &times]() {
                std::shared_ptr<SchdelerSafeUnit> ssup = std::make_shared<SchdelerSafeUnit>(s);
                uint32 count = times;
                for (uint32 i = 0; i < count; i++) {
                     auto g = [&times, &s](std::shared_ptr<SchdelerSafeUnit> &sp) {
                       FUNC_TRACE
                       std::this_thread::sleep_for(std::chrono::seconds(1));
                       times--;
                       if (0 == times) {
                           log_debug("over");
                           s.stop();
                       }
                     };
                     ssup->post(std::bind(g, ssup));
                }
            }, 1);
}

#endif
