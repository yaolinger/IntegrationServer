#ifndef UTILS_SCHEDULER_UNIT_HPP
#define UTILS_SCHEDULER_UNIT_HPP

#include <atomic>
#include <functional>
#include <list>
#include <mutex>

#include "noncopyable.hpp"
#include "scheduler_base.hpp"

NS_UTILS_BEGIN

// 调度单元
// 禁止拷贝, 非线程安全
class SchdeulerUnit : public UnitBase, Noncopyable {
public:
    SchdeulerUnit(UnitFunc func): m_func(func) {}

    void complete() { m_func(); }

private:
    UnitFunc m_func;           // 执行函数
};

// 线程安全调度单元
// 禁止拷贝
class SchdelerSafeUnit : public UnitBase, Noncopyable {
enum {
    STATUS_EMPTY = 0,  // 无任务执行状态
    STATUS_RUN = 1,   // 执行状态
};
public:
    SchdelerSafeUnit() {}
    ~SchdelerSafeUnit() {}

    SchdelerSafeUnit(SchdelerSafeUnit&& unit) = delete;

    void postFunc(UnitFunc func);

    void runFunc();

    void complete();

private:
    std::atomic<int> m_status;                            // 当前状态
    std::mutex m_waitMutex;                               // 等待队列锁
    std::list<UnitFunc> m_waitList;                       // 等待队列
    std::mutex m_accessMutex;                             // 执行队列锁
    std::list<UnitFunc> m_accessList;                     // 执行队列
};


NS_UTILS_END

#endif
