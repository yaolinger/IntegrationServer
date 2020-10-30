#include "log.hpp"

#include <iostream>
#include <stdarg.h>

#include "time_helper.hpp"

NS_UTILS_BEGIN

uint32 Logger::m_logControl = 0;

void Logger::init() {
	setControl(LOG_CONTROL_TO_CONSOLE | LOG_CONTROL_TRACE | LOG_CONTROL_DEBUG | LOG_CONTROL_INFO | LOG_CONTROL_WARN | LOG_CONTROL_ERROR);
}

void Logger::setControl(uint32 control) {
	m_logControl |= control;
}

void Logger::unsetControl(uint32 control) {
	m_logControl ^= control;
}

void Logger::output(LogLevel level, const char* file, int32 line, const char * func, const char* format, ...) {
	if (!isOutputLevel(level)) {
		return;
	}

	switch (level) {
		case LOG_LEVEL_TRACE:
			std::cout << OUTPUT_TRACE_COLOUR << "[TRACE] ";
			break;
		case LOG_LEVEL_DEBUG:
			std::cout << OUTPUT_DEBUG_COLOUR << "[DEBUG] ";
			break;
		case LOG_LEVEL_INFO:
			std::cout << OUTPUT_INFO_COLOUR << "[INFO ] ";
			break;
		case LOG_LEVEL_WARN:
			std::cout << OUTPUT_WARN_COLOUR << "[WARN ] ";
			break;
		case LOG_LEVEL_ERROR:
			std::cout << OUTPUT_ERROR_COLOUR << "[ERROR] ";
			break;
		default:
			return;
	}

	std::string logHead = TimeHelper::getNowTimeStr() + " [" + std::string(file) + ":" + std::to_string(line) + ":" + std::string(func) +"] ";

	char logMsg[LOG_MAX_LEN] = {0};
	va_list args;
	va_start(args, format);
	std::vsnprintf(logMsg, LOG_MAX_LEN - 1, format, args);
	va_end(args);

	std::cout << logHead << logMsg << OUTPUT_NULL_COLOUR << std::endl;
}

bool Logger::isOutputLevel(LogLevel level) {
	switch (level) {
		case LOG_LEVEL_TRACE:
			return (m_logControl & LOG_CONTROL_TRACE) == LOG_CONTROL_TRACE;
		case LOG_LEVEL_DEBUG:
			return (m_logControl & LOG_CONTROL_DEBUG) == LOG_CONTROL_DEBUG;
		case LOG_LEVEL_INFO:
			return (m_logControl & LOG_CONTROL_INFO) == LOG_CONTROL_INFO;
		case LOG_LEVEL_WARN:
			return (m_logControl & LOG_CONTROL_WARN) == LOG_CONTROL_WARN;
		case LOG_LEVEL_ERROR:
			return (m_logControl & LOG_CONTROL_ERROR) == LOG_CONTROL_ERROR;
		default:
			return false;
	}
}

NS_UTILS_END
