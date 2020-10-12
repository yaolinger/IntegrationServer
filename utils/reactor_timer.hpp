#ifndef UTILS_REACTOR_TIMER_HPP
#define UTILS_REACTOR_TIMER_HPP

#include <atomic>
#include <list>
#include <map>
#include <mutex>

#include "macros.hpp"
#include "scheduler_base.hpp"

NS_UTILS_BEGIN

// 基于反应堆的定时器
class ReactorTimer {
public:
    ReactorTimer(ReactorPtr pReactor);
    ~ReactorTimer();

public:
    // 定时执行
    void expiresFunc(uint32 seconds, std::function<void()> func);
    // 取消func
    void cancelFunc();
private:
    // 获取计时器任务
    static void getTimerTask(std::list<UnitPtr>& taskList);

private:
    static std::once_flag s_once;                               // call_once
    static std::mutex s_unitMutex;                              // 待执行函数锁
    static std::map<uint32, std::list<UnitPtr>> s_unitMap;      // unit map(TODO::后续采用小根堆)

private:
    ReactorPtr m_reactor;              // 反应堆
    std::atomic<bool> m_cancelSign;    // 取消标识
};

NS_UTILS_END

#endif
