#include "error.hpp"

#include <errno.h>
#include <string.h>

NS_UTILS_BEGIN

std::string SystemErrno::getSystemError() {
    std::string result = std::string("errno[" + std::to_string(errno) + ":" + strerror(errno)+ "]");
    return result;
}

NS_UTILS_END

