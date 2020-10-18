#ifndef UTILS_SCHEDULER_SAFE_UNIT_HPP
#define UTILS_SCHEDULER_SAFE_UNIT_HPP

#include <atomic>
#include <functional>
#include <list>
#include <mutex>

#include "noncopyable.hpp"
#include "scheduler_base.hpp"
#include "scheduler_wrapped_handler.hpp"
#include "log.hpp"

NS_UTILS_BEGIN

// 线程安全调度单元
// 禁止拷贝 禁止移动构造
class SchdelerSafeUnit : public Noncopyable {
class SafeUnit : public UnitBase, Noncopyable {
friend class SchdelerSafeUnit;
public:
    void complete() { func(); }

private:
    bool access;                                        // 是否正在执行(false, ture)
    std::mutex listMutex;                               // 队列锁
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

    void cancelUnits(UnitFunc callback);

    // wrap:多参数function包装器(●ˇ∀ˇ●)
    template<typename Handler>
    SchedulerWrappedHandler<SchdelerSafeUnit, Handler> wrap(Handler handler) {
        return SchedulerWrappedHandler<SchdelerSafeUnit, Handler>(*this, handler);
    }

    std::atomic<bool> m_cancelFlag;                       // 关闭标识
    UnitFunc m_cancelCallback;                            // 关闭回调
    SchedulerBase& m_scheduler;                           // 调度器
    std::shared_ptr<SafeUnit> m_unitPtr;                  // 执行单元
};
typedef std::shared_ptr<SchdelerSafeUnit> SchdelerSafeUnitPtr;

NS_UTILS_END

#endif
