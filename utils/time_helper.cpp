#include "time_helper.hpp"

#include <ctime>
#include <iomanip>
#include <sstream>

NS_UTILS_BEGIN

int32 TimeHelper::m_initTime;

steadyClock::time_point TimeHelper::m_start;

void TimeHelper::init(int now) {
	m_start = steadyClock::now();
	m_initTime = now == 0 ? getSystemCurrentSecond(): now;
}

int32 TimeHelper::getCurrentSecond() {
	return std::chrono::duration_cast<seconds>(steadyClock::now() - m_start).count() + m_initTime;
}

int64 TimeHelper::getCurrentMilliseconds() {
	return std::chrono::duration_cast<milliseconds>(steadyClock::now() - m_start).count() + int64(m_initTime) * 1000;
}

//获取当前系统时间
//typedef int32/int64 time_t
//time_t time(arg) or time_t to_time_t()
int32 TimeHelper::getSystemCurrentSecond() {
	//return (int32)time(nullptr);
	//return (int32)systemClock::to_time_t(systemClock::now());
    std::chrono::system_clock::time_point start; // 初始时间,默认值为 1970-01-01 08:00:00
    return std::chrono::duration_cast<seconds>(systemClock::now() - start).count();
}

int64 TimeHelper::getSystemCurrentMillisecond() {
    std::chrono::system_clock::time_point start; // 初始时间,默认值为 1970-01-01 08:00:00
    return (int64)std::chrono::duration_cast<milliseconds>(systemClock::now() - start).count();
}

std::string TimeHelper::getTimeStr(int32 ts) {
	std::time_t t = ts;
	std::stringstream os;
	os << std::put_time(std::localtime(&t), "%Y/%m/%d %T");
	return os.str();
}

std::string TimeHelper::getNowTimeStr() {
    return getTimeStr(getCurrentSecond());
}

NS_UTILS_END

