#ifndef UTILS_ERROR_HPP
#define UTILS_ERROR_HPP

#include "macros.hpp"

#include <string>

NS_UTILS_BEGIN

// 获取系统错误
#define GET_SYSTEM_ERRNO_INFO SystemErrno::getSystemError()
// 清除系统错误
#define CLEAR_SYSTEM_ERRNO \
    errno = 0

// 系统错误
class SystemErrno {
public:
    // 获取系统错误
    static std::string getSystemError();
};

NS_UTILS_END

#endif
