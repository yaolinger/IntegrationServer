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
    uint32 safeTimes = 10;

    // test SchdeulerUnit
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

    // test SchdelerSafeUnit
    pool.createThread([&s, &safeTimes]() {
                std::shared_ptr<SchdelerSafeUnit> ssup = std::make_shared<SchdelerSafeUnit>(s);
                uint32 count = safeTimes;
                for (uint32 i = 0; i < count; i++) {
                     auto g = [&safeTimes, &s](std::shared_ptr<SchdelerSafeUnit> sp) {
                            safeTimes--;
                            log_info("Run safe unit times[%u]", safeTimes);
                            if (0 == safeTimes) {
                                // 最后测试wrap
                                typedef std::function<void(uint32 leftTimes, std::string str)> TestFunc;
                                TestFunc overFunc = [&s](uint32 leftTimes, std::string str) {
                                            log_info("%s left times[%u].", str.c_str(), leftTimes);
                                            s.stop();
                                };

                                auto overHandler = sp->wrap(overFunc);
                                auto func = [&safeTimes](SchedulerWrappedHandler<SchdelerSafeUnit, TestFunc> handler, std::shared_ptr<SchdelerSafeUnit> sp) {
                                    std::string str = "Safe unit test over ";
                                    handler(safeTimes, str);
                                };

                                // 提交结束任务
                                UnitPtr pUnit = std::make_shared<SchdeulerUnit>(std::bind(func, overHandler, sp));
                                s.post(pUnit);
                            }
                     };
                     ssup->post(std::bind(g, ssup));
                }
            }, 1);
}

#endif
