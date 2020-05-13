#ifndef OP_CMD_HPP
#define OP_CMD_HPP

namespace op_cmd {
    // 协议枚举
    enum {
%for key, value in Msgs:
    ${value} = ${key},
%endfor
    };

}

#endif
