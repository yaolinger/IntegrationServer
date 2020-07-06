#include "scheduler_list.hpp"

NS_UTILS_BEGIN

void SchedulerList::wait() {
    std::unique_lock<std::mutex> lock(m_mutex);
    m_cond.wait(lock);
}

void SchedulerList::notify(bool all) {
    if (all) {
        m_cond.notify_all();
    } else {
        m_cond.notify_one();
    }
}

void SchedulerList::pushWithNotify(UnitPtr& ptr) {
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_taskList.push_back(ptr);
    }
    m_cond.notify_one();
}

void SchedulerList::pushWithoutNofity(UnitPtr& ptr) {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_taskList.push_back(ptr);
}

UnitPtr SchedulerList::getTast() {
    std::lock_guard<std::mutex> lock(m_mutex);
    if (m_taskList.size() == 0) {
        return NULL;
    }
    UnitPtr result = *m_taskList.begin();
    m_taskList.pop_front();
    return result;
}

void SchedulerList::getTaskList(std::list<UnitPtr>& list) {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_taskList.swap(list);
}

NS_UTILS_END
