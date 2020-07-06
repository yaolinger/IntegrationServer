#ifndef UTILS_SCHEDULER_LIST_HPP
#define UTILS_SCHEDULER_LIST_HPP

#include <condition_variable>
#include <list>
#include <mutex>

#include "macros.hpp"
#include "scheduler_base.hpp"

NS_UTILS_BEGIN

// 调度器任务队列
class SchedulerList {
public:
    // 等待数据
    void wait();

    // 激活一次(true:全部激活, false:激活一个)
    void notify(bool all);

    // 添加任务(激活)
    void pushWithNotify(UnitPtr& ptr);

    // 添加任务(不激活)
    void pushWithoutNofity(UnitPtr& ptr);

    // 获取任务(一个)
    UnitPtr getTast();

    // 获取任务(全部)
    void getTaskList(std::list<UnitPtr>& list);

    // 获取任务数量
    uint32 getTaskSize() { return m_taskList.size(); }
private:
    std::mutex m_mutex;
    std::condition_variable m_cond;
    std::list<UnitPtr> m_taskList;
};
typedef std::shared_ptr<SchedulerList> SchedulerListPtr;


NS_UTILS_END

#endif

