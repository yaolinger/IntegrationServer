#ifndef UTILS_TIME_MANAGER_HPP
#define UTILS_TIME_MANAGER_HPP

#include<functional>
#include<map>

#include <boost/asio/deadline_timer.hpp>
#include <boost/asio/io_service.hpp>
#include <boost/system/error_code.hpp>

#include "macros.hpp"

NS_UTILS_BEGIN

// 基础定时器
class TimerBase : public std::enable_shared_from_this<TimerBase> {
    enum TimerState {
        TS_CLOSE = 0,
        TS_START = 1,
    };

typedef std::function<void()> FuncType;

public:
    TimerBase(uint64 timeId) : m_over(false), m_timeId(timeId) {}
    ~TimerBase() {};

public:
    // 取消定时器
    void cancel() { m_over = true; }

    // 定时器是否结束
    bool isOver() { return m_over; }

    // 获取时间id
    uint64 getTimerId()  { return m_timeId; }

    // 注册定时器
    bool afterFunc(uint32 seconds, FuncType func);

    // 执行func
    void doComplete();

    // 获取执行时间;
    uint32 getRunTime() { return m_start; }
private:
    bool m_over;      // 定时器结束表示
    uint64 m_timeId;  // 定时器id
    FuncType m_func = NULL;  // 执行函数
    uint32 m_start = 0;       // 开始时间

    TimerState m_state = TimerState::TS_CLOSE; // 定时器状态(暂时没用)
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
    static std::shared_ptr<boost::asio::deadline_timer> m_timer; // boost 定时器
};

NS_UTILS_END

#endif
