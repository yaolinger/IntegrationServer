#include "scheduler_safe_unit.hpp"

NS_UTILS_BEGIN

SchdelerSafeUnit::SchdelerSafeUnit(SchedulerBase& scheduler)
    : m_scheduler(scheduler) {
    m_unitPtr = std::make_shared<SafeUnit>();
    m_unitPtr->access = false;
    m_unitPtr->cancelFlag = false;
    m_unitPtr->func = std::bind(&SchdelerSafeUnit::doComplete, this);
}

void SchdelerSafeUnit::post(UnitFunc func) {
    std::lock_guard<std::mutex> lock(m_unitPtr->listMutex);
    if (m_unitPtr->cancelFlag) {
        return;
    }
    postWithoutLock(func);
}

void SchdelerSafeUnit::postWithoutLock(UnitFunc func) {
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

    {
        std::lock_guard<std::mutex> lock(m_unitPtr->listMutex);
        m_unitPtr->waitList.swap(m_unitPtr->accessList);
        if (m_unitPtr->accessList.size() == 0) {
            m_unitPtr->access = false;
        } else {
            UnitPtr pUnit = m_unitPtr;
            m_scheduler.post(pUnit);
        }
    }
}

void SchdelerSafeUnit::cancelUnits(UnitFunc logicFunc) {
    std::lock_guard<std::mutex> lock(m_unitPtr->listMutex);
    if (m_unitPtr->cancelFlag) {
        return;
    }
    postWithoutLock(std::bind([](UnitFunc func) {
                func();
            }, logicFunc));
    m_unitPtr->cancelFlag = true;
}

NS_UTILS_END

