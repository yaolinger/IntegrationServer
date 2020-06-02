#ifndef UTILS_LOG_HELPER_HPP
#define UTILS_LOG_HELPER_HPP

#include <string>

#include "macros.hpp"
#include "time_helper.hpp"

NS_UTILS_BEGIN

// 函数执行日志
struct FuncTrace {
	FuncTrace(const char* file, const char* func, int32 line)
		: m_file(file), m_func(func), m_line(line) {
		Logger::output(LOG_LEVEL_TRACE, m_file.c_str(), m_line, m_func.c_str(), "~~~~~~~~~~~~[%s] start run ~~~~~~~~~~~~~~~~~~~", m_func.c_str());
	}
	~FuncTrace() {
		Logger::output(LOG_LEVEL_TRACE, m_file.c_str(), m_line, m_func.c_str(), "~~~~~~~~~~~~[%s] end, run[%d(us)]~~~~~~~~~~~~", m_func.c_str(), sw.durationUs());
	}

	StopWatch sw;
	std::string m_file;
	std::string m_func;
	int32 m_line;
};

NS_UTILS_END

#define FUNC_TRACE \
		UTILS::FuncTrace ft(__FILE__, __FUNCTION__, __LINE__);

#endif
