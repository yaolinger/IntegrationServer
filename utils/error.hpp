#ifndef UTILS_ERROR_HPP
#define UTILS_ERROR_HPP

#include "macros.hpp"

#include <string>

NS_UTILS_BEGIN

// 获取系统错误
#define GET_SYSTEM_ERRNO_INFO SystemErrno::getSystemError()

// 系统错误
class SystemErrno {
public:
    // 获取系统错误
    static std::string getSystemError();
};

NS_UTILS_END

#endif
