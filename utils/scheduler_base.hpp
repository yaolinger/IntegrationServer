#ifndef UTILS_SCHEDULER_BASE_HPP
#define UTILS_SCHEDULER_BASE_HPP

#include <functional>

#include "macros.hpp"

NS_UTILS_BEGIN

// 单元基础
class UnitBase {
public:
    virtual void complete() = 0;
};

// 单元函数
typedef std::function<void(void)> UnitFunc;


NS_UTILS_END

#endif
