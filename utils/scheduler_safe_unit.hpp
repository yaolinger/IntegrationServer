#ifndef UTILS_SCHEDULER_SAFE_UNIT_HPP
#define UTILS_SCHEDULER_SAFE_UNIT_HPP

#include <atomic>
#include <functional>
#include <list>
#include <mutex>

#include "noncopyable.hpp"
#include "scheduler_base.hpp"
#include "log.hpp"

NS_UTILS_BEGIN

// 线程安全调度单元
// 禁止拷贝 禁止移动构造
class SchdelerSafeUnit : public Noncopyable {
enum {
    STATUS_EMPTY = 0,  // 无任务执行状态
    STATUS_RUN = 1,   // 执行状态
};

class SafeUnit : public UnitBase, Noncopyable {
friend class SchdelerSafeUnit;
public:
    void complete() { func(); }

private:
    std::atomic<int> status;                            // 当前状态
    std::mutex waitMutex;                               // 等待队列锁
    std::list<UnitFunc> waitList;                       // 等待队列
    std::list<UnitFunc> accessList;                     // 执行队列
    UnitFunc func;                                      // 执行函数
};

public:
    SchdelerSafeUnit(SchedulerBase& scheduler);
    ~SchdelerSafeUnit() {}

    SchdelerSafeUnit(SchdelerSafeUnit&& unit) = delete;

    void post(UnitFunc func);

    void doComplete();

    SchedulerBase& m_scheduler;                           // 调度器
    std::shared_ptr<SafeUnit> m_unitPtr;                  // 执行单元
};
typedef std::shared_ptr<SchdelerSafeUnit> SchdelerSafeUnitPtr;

NS_UTILS_END

#endif
