#ifndef UTILS_SCHEDULER_WRAPPED_HANDLER_HPP
#define UTILS_SCHEDULER_WRAPPED_HANDLER_HPP

#include <functional>
#include "macros.hpp"

NS_UTILS_BEGIN

// 多参数function包装器
// 利用仿函数投递任务
// 解决多参数function无法直接投递问题
template<typename Scheduler, typename Handler>
class SchedulerWrappedHandler {
public:
    SchedulerWrappedHandler(Scheduler& scheduler, Handler handler) : m_scheduler(scheduler), m_handler(handler) {
    }

public:
    // 目前最多支持6个参数(●'◡'●)
    void operator () () {
        m_scheduler.post(m_handler);
    }

    template<typename P1>
    void operator () (const P1& p1) {
       m_scheduler.post(std::bind(m_handler, p1));
    }

    template<typename P1, typename P2>
    void operator () (const P1& p1, const P2& p2) {
        m_scheduler.post(std::bind(m_handler, p1, p2));
    }

    template<typename P1, typename P2, typename P3>
    void operator () (const P1& p1, const P2& p2, const P3& p3) {
        m_scheduler.post(std::bind(m_handler, p1, p2, p3));
    }

    template<typename P1, typename P2, typename P3, typename P4>
    void operator () (const P1& p1, const P2& p2, const P3& p3, const P4& p4) {
        m_scheduler.post(std::bind(m_handler, p1, p2, p3, p4));
    }

    template<typename P1, typename P2, typename P3, typename P4, typename P5>
    void operator () (const P1& p1, const P2& p2, const P3& p3, const P4& p4, const P5& p5) {
        m_scheduler.post(std::bind(m_handler, p1, p2, p3, p4, p5));
    }

    template<typename P1, typename P2, typename P3, typename P4, typename P5, typename P6>
    void operator () (const P1& p1, const P2& p2, const P3& p3, const P4& p4, const P5& p5, const P6& p6) {
        m_scheduler.post(std::bind(m_handler, p1, p2, p3, p4, p5, p6));
    }

private:
    Scheduler& m_scheduler;
    Handler m_handler;
};

NS_UTILS_END

#endif
