#ifndef UTILS_REACTOR_HPP
#define UTILS_REACTOR_HPP

#include "scheduler_base.hpp"

NS_UTILS_BEGIN

// 测试用reactor (只模仿阻塞效果)
class FakeReactor : public ReactorBase {
public:
    void reactorWait(std::list<UnitPtr>& taskList);
};

NS_UTILS_END

#endif
