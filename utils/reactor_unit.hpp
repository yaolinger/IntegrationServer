#ifndef UTILS_REACTOR_UNIT_HPP
#define UTILS_REACTOR_UNIT_HPP

#include "macros.hpp"
#include "scheduler_base.hpp"

NS_UTILS_BEGIN

// 声明
class ReactorEpoll;
// 反应堆接口
typedef std::function<void(std::string& error, uint32 transBytes)> ReactorUnitFunc;

// reactor 调度单元
class ReactorUnit : public UnitBase {
friend class ReactorEpoll;

public:
    ReactorUnit(ReactorUnitFunc func) : m_transBytes(0), m_func(func) {
    }
    ~ReactorUnit() {}

public:
    void complete();

private:
    std::string m_error;        // 错误信息
    uint32 m_transBytes;        // 传输字节数量
    ReactorUnitFunc m_func;     // 执行函数
};

typedef std::shared_ptr<ReactorUnit> ReactorUnitPtr;

NS_UTILS_END

#endif
