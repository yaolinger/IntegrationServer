#ifndef UTILS_SCHEDULE_HPP
#define UTILS_SCHEDULE_HPP

#include <atomic>
#include <condition_variable>
#include <map>
#include <mutex>
#include <thread>

#include "scheduler_base.hpp"
#include "scheduler_list.hpp"

NS_UTILS_BEGIN

// SCHEDULER_MODE_PRODUCER 生产者模式
// 线程分配: reactorThread(1) workThread(0)
// 场景: reactor(epoll/select/poll),构建定时器
// SCHEDULER_MODE_CONSUMER 消费者模式
// 线程分配: reactorThread(0) workThread(1~n)
// 场景: 使用工作线程写日志
// SCHEDULER_MODE_PRODUCER | SCHEDULER_MODE_CONSUMER 双模式
// 线程分配: reactorThread(1) workThread(1~n)
// 场景: reactor(epoll/select/poll)socket监听, 工作线程socket 操作(send, recv, close)
#define SCHEDULER_MODE_PRODUCER 1    // 生产者模式
#define SCHEDULER_MODE_CONSUMER 2    // 消费者模式

// 调度器(任务生产,消费,分配)
// 固化线程绑定: reactorThreadFunc(epoll/select/poll), workThreadFunc(socket operator)
// 线程数量: reactorThread(0~1), workThread(0~n)
class Scheduler : public SchedulerBase {
public:
    Scheduler(uint32 mode);
    ~Scheduler() {}

    // 开始工作
    void start();

    // 停止工作
    void stop();

    // 提交任务
    void post(UnitPtr& ptr);

    // work线程接口
    void workThreadFunc();

    // 反应堆线程接口
    void reactorThreadFunc();
private:
    // 生产者
    void producerFunc();

    // 消费者
    void consumerFunc();

    // 工作开始等待
    void startWait();

private:
    // 工作流程控制
    uint32 m_mode;                                                     // 使用模式
    bool m_start;
    std::mutex m_startMutex;                                           // 开始工作锁
    std::condition_variable m_startCond;                               // 开始工作条件变量

    // reactor数据
    std::atomic<bool> m_reactorStart;                                  // reactor开始工作标识

    // work数据
    std::atomic<bool> m_workStart;                                     // work工作标识
    SchedulerList m_workList;                                          // work队列
};

NS_UTILS_END

#endif
