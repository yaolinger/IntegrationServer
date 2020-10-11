#ifndef UTILS_REACTOR_HPP
#define UTILS_REACTOR_HPP

#include "scheduler_base.hpp"

NS_UTILS_BEGIN

// 测试用reactor (只模仿阻塞效果)
class ReactorFake : public ReactorBase {
public:
    void reactorWait(std::list<UnitPtr>& taskList, int32 timeout);

    const std::string& getError() { return m_error; }

private:
    std::string m_error = "fake";
};

NS_UTILS_END

#endif
