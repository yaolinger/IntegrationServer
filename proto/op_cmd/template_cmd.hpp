#ifndef PROTOCPP_OP_CMD_HPP
#define PROTOCPP_OP_CMD_HPP

#include "utils/macros.hpp"

NS_PROTOCPP_BEGIN
    // 协议枚举
    enum {
%for key, value in Msgs:
		${value} = ${key},
%endfor
    };

NS_PROTOCPP_END

#endif
