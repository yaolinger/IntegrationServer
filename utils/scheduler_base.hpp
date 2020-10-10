#ifndef UTILS_SCHEDULER_BASE_HPP
#define UTILS_SCHEDULER_BASE_HPP

#include <functional>
#include <list>
#include <memory>
#include <string>

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
typedef std::shared_ptr<SchedulerBase> SchedulerPtr;

// 反应堆基础
class ReactorBase {
public:
    virtual void reactorWait(std::list<UnitPtr>& taskList, int32 timeout) = 0;

    virtual const std::string& getError() = 0;
};

typedef std::shared_ptr<ReactorBase> ReactorPtr;

// 反应堆事件(发起动作)
enum REACTOR_EVENT {
    REACTOR_EVENT_READ = 1,   // 读事件
    REACTOR_EVENT_WRITE = 2,  // 写事件
};

// 反应堆激活事件(激活动作)
enum REACTOR_ACTIVATE_EVENT {
    REACTOR_ACTIVATE_EVENT_READ = 0x01,   // 读激活事件
    REACTOR_ACTIVATE_EVENT_WRITE = 0x02,  // 写激活事件
    REACTOR_ACTIVATE_EVENT_ERROR = 0x04,  // 错误激活事件
};

NS_UTILS_END

#endif
