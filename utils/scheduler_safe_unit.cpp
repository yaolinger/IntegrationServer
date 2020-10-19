#include "scheduler_safe_unit.hpp"

NS_UTILS_BEGIN

SchdelerSafeUnit::SchdelerSafeUnit(SchedulerBase& scheduler)
    : m_scheduler(scheduler) {
    m_unitPtr = std::make_shared<SafeUnit>();
    m_unitPtr->func = std::bind(&SchdelerSafeUnit::doComplete, this);
    m_unitPtr->access = false;
    m_cancelFlag.store(false);
}

void SchdelerSafeUnit::post(UnitFunc func) {
    if (m_cancelFlag.load()) {
        return;
    }

    std::lock_guard<std::mutex> lock(m_unitPtr->listMutex);
    if (m_unitPtr->access == false) {
        m_unitPtr->access = true;
        m_unitPtr->accessList.push_back(func);
        UnitPtr pUnit = m_unitPtr;
        m_scheduler.post(pUnit);
    } else {
        m_unitPtr->waitList.push_back(func);
    }
}

void SchdelerSafeUnit::doComplete() {
    std::list<UnitFunc> runList;
    m_unitPtr->accessList.swap(runList);

    for (auto& iter : runList) {
        iter();
    }

    bool over = false;
    {
        std::lock_guard<std::mutex> lock(m_unitPtr->listMutex);
        m_unitPtr->waitList.swap(m_unitPtr->accessList);

        if (m_unitPtr->accessList.size() == 0) {
            m_unitPtr->access = false;
            if (m_cancelFlag.load()) {
                over = true;
            }
        } else {
            UnitPtr pUnit = m_unitPtr;
            m_scheduler.post(pUnit);
        }
    }
    if (over) {
        m_cancelCallback();
    }
}

void SchdelerSafeUnit::cancelUnits(UnitFunc logicFunc) {
    m_cancelCallback = logicFunc;
    m_cancelFlag.store(true);
    UnitPtr pUnit = m_unitPtr;
    m_scheduler.post(pUnit);
}

NS_UTILS_END

