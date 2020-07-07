#ifndef UTILS_SCHEDULER_BASE_HPP
#define UTILS_SCHEDULER_BASE_HPP

#include <functional>
#include <list>
#include <memory>

#include "macros.hpp"

NS_UTILS_BEGIN

// 单元函数
typedef std::function<void(void)> UnitFunc;

// 单元基础
class UnitBase {
public:
    virtual void complete() = 0;
};
typedef std::shared_ptr<UnitBase> UnitPtr;

// 调度器基础
class SchedulerBase {
public:
    virtual void start() = 0;

    virtual void stop() = 0;

    virtual void post(UnitPtr& pUnit) = 0;
};

// 反应堆基础
class ReactorBase {
public:
    virtual void reactorWait(std::list<UnitPtr>& taskList) = 0;
};

typedef std::shared_ptr<ReactorBase> ReactorPtr;

NS_UTILS_END

#endif
