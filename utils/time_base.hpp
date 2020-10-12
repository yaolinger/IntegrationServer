#ifndef UTILS_TIME_BASE_HPP
#define UTILS_TIME_BASE_HPP

#include <chrono>
#include "macros.hpp"

NS_UTILS_BEGIN

// 单调时钟
typedef std::chrono::steady_clock steadyClock;
// 系统时钟
typedef std::chrono::system_clock systemClock;

// 小时(h)  std::ratio<3600, 1>
typedef std::chrono::hours hours;
// 分钟(m)  std::ratio<60, 1>
typedef std::chrono::minutes minutes;
// 秒(s)    std::ratio<1, 1>
typedef std::chrono::seconds seconds;
// 毫秒(ms) std::ratio<1, 1000>
typedef std::chrono::milliseconds milliseconds;
// 微妙(us) std::ratio<1, 1000000>
typedef std::chrono::microseconds microseconds;

// 计时器粒度(500 毫秒)
#define TIMER_MANAGER_DURING boost::posix_time::millisec(1000)

NS_UTILS_END

#endif
