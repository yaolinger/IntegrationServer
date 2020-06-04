#ifndef UTILS_TIME_MANAGER_HPP
#define UTILS_TIME_MANAGER_HPP

#include<functional>
#include<map>

#include <boost/asio/deadline_timer.hpp>
#include <boost/asio/io_service.hpp>
#include <boost/system/error_code.hpp>

#include "macros.hpp"

NS_UTILS_BEGIN

// 基础计时器
class TimerBase {
    enum TimerState {
        TS_CLOSE = 0,
        TS_START = 1,
    };

typedef std::function<void(bool isStart)> FuncType;

public:
    TimerBase(uint64 timeId) : m_over(false), m_timeId(timeId) {}
    ~TimerBase() {};

public:
    // 取消计时器
    void Cancel() { m_over = true; }

    // 计时器是否结束
    bool isOver() { return m_over; }

    // 获取时间id
    uint64 getTimerId()  { return m_timeId; }

    // 注册计时器
    bool registrate(FuncType func);

    // 执行func
    void doComplete();
public:
    // 获取下次执行时间
    virtual void getNextTime() = 0;

    // 计算时间
    virtual void calcTime() = 0;

public:
    bool m_over;      // 计时器结束表示
    uint64 m_timeId;  // 计时器id
    FuncType m_func = NULL ;  // 执行函数

    TimerState m_state = TimerState::TS_CLOSE; // 计时器状态
};

typedef std::shared_ptr<TimerBase> TimerPtr;


// 时间管理器
// 基于boost asio deadline_time
// 未使用boost asio io_service strand
// 单线程使用(可放入主逻辑线程)
class TimerManager {
public:
    static void init(boost::asio::io_service& ios);

    static void cancel();

    static bool registrate(TimerPtr& pTimer);

    static void onTimer(const boost::system::error_code& ec);
private:
    static std::multimap<uint32, TimerPtr> m_timerMap;  // nextTime : timer

private:
    static std::shared_ptr<boost::asio::deadline_timer> m_timer; // boost 计时器
};

NS_UTILS_END

#endif
