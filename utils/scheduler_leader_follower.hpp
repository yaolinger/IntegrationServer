#ifndef UTILS_SCHEDULE_LEADER_FOLLOWER_HPP
#define UTILS_SCHEDULE_LEADER_FOLLOWER_HPP

#include <atomic>
#include <condition_variable>
#include <map>
#include <mutex>
#include <thread>

#include "scheduler_base.hpp"
#include "scheduler_list.hpp"
#include "scheduler_unit.hpp"

NS_UTILS_BEGIN

// reactor 模式
#define SCHEDULE_LF_MODE_REACTOR 0x01
// 消息队列模式
#define SCHEDULE_LF_MODE_TASK_LIST 0x02

// 调度器(leader/follower模式)
class SchedulerLF : public SchedulerBase {
public:
    SchedulerLF(uint32 mode);
    ~SchedulerLF();

    // 开始工作
    void start();

    // 停止工作
    void stop();

    // 提交任务
    void post(UnitPtr& ptr);

    // 线程工作函数
    void run();
private:
    // reactor 网络事件
    void reactorFunc();

    // 计时器
    void timerFunc();

    // 其他事件
    void otherFunc();

    // 工作开始等待
    void startWait();

private:
    uint32 m_mode;                                                     // 模式
    bool m_start;                                                      // 开始标识
    std::mutex m_startMutex;                                           // 开始工作锁
    std::condition_variable m_startCond;                               // 开始工作条件变量

    std::atomic<bool> m_work;                                          // 开始标识
    SchedulerList m_workList;                                          // work队列

    ReactorPtr m_reactor;                                              // reactor
    SchdeulerUnitPtr m_reactorUnit;                                    // 切换reactor线程(reactor执行)
};

NS_UTILS_END

#endif
