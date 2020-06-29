#include "scheduler_unit.hpp"

NS_UTILS_BEGIN

void SchdelerSafeUnit::postFunc(UnitFunc func) {
    if (STATUS_EMPTY == m_status.load()) {
        std::lock_guard<std::mutex> lock(m_accessMutex);
        m_accessList.push_back(func);
        m_status.store(STATUS_RUN);
    } else if (STATUS_RUN == m_status.load()) {
        std::lock_guard<std::mutex> lock(m_waitMutex);
        m_waitList.push_back(func);
    }
}

void SchdelerSafeUnit::runFunc() {
    std::list<UnitFunc> runList;
    {
        std::lock_guard<std::mutex> lock(m_accessMutex);
        m_accessList.swap(runList);
    }

    for (auto& iter : runList) {
        iter();
    }

    complete();
}

void SchdelerSafeUnit::complete() {
    std::list<UnitFunc> accessList;
    {
        std::lock_guard<std::mutex> lock(m_waitMutex);
        m_waitList.swap(accessList);
    }

    if (accessList.size() > 0) {
        std::lock_guard<std::mutex> lock(m_accessMutex);
        m_accessList.swap(accessList);
    } else {
        m_status.store(STATUS_EMPTY);
    }
}

NS_UTILS_END

