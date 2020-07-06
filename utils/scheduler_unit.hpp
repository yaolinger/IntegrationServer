#ifndef UTILS_SCHEDULER_UNIT_HPP
#define UTILS_SCHEDULER_UNIT_HPP

#include <atomic>
#include <functional>
#include <list>
#include <mutex>

#include "noncopyable.hpp"
#include "scheduler_base.hpp"

NS_UTILS_BEGIN

// 调度单元
// 禁止拷贝, 非线程安全
class SchdeulerUnit : public UnitBase, Noncopyable {
public:
    SchdeulerUnit(UnitFunc func): m_func(func) {}

    void complete() { m_func(); }

private:
    UnitFunc m_func;           // 执行函数
};
typedef std::shared_ptr<SchdeulerUnit> SchdeulerUnitPtr;

NS_UTILS_END

#endif
