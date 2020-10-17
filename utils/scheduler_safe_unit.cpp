#include "scheduler_safe_unit.hpp"

NS_UTILS_BEGIN

SchdelerSafeUnit::SchdelerSafeUnit(SchedulerBase& scheduler)
    : m_scheduler(scheduler) {
    m_unitPtr = std::make_shared<SafeUnit>();
    m_unitPtr->func = std::bind(&SchdelerSafeUnit::doComplete, this);
    m_unitPtr->status.store(STATUS_EMPTY);
    m_cancelFlag.store(false);
}

void SchdelerSafeUnit::post(UnitFunc func) {
    if (m_cancelFlag.load()) {
        return;
    }

    if (STATUS_EMPTY == m_unitPtr->status.exchange(STATUS_RUN)) {
        m_unitPtr->accessList.push_back(func);
        UnitPtr pUnit = m_unitPtr;
        m_scheduler.post(pUnit);
    } else {
        std::lock_guard<std::mutex> lock(m_unitPtr->waitMutex);
        m_unitPtr->waitList.push_back(func);
    }
}

void SchdelerSafeUnit::doComplete() {
    std::list<UnitFunc> runList;
    m_unitPtr->accessList.swap(runList);

    for (auto& iter : runList) {
        iter();
    }

    {
        std::lock_guard<std::mutex> lock(m_unitPtr->waitMutex);
        m_unitPtr->waitList.swap(m_unitPtr->accessList);
    }

    if (m_unitPtr->accessList.size() == 0) {
        m_unitPtr->status.store(STATUS_EMPTY);
        if (m_cancelFlag.load()) {
            m_cancelCallback();
        }
    } else {
        UnitPtr pUnit = m_unitPtr;
        m_scheduler.post(pUnit);
    }
}

void SchdelerSafeUnit::cancelUnits(UnitFunc logicFunc) {
    m_cancelFlag.store(true);
    m_cancelCallback = logicFunc;
    UnitPtr pUnit = m_unitPtr;
    m_scheduler.post(pUnit);
}

NS_UTILS_END

