#include "scheduler_leader_follower.hpp"

NS_UTILS_BEGIN

SchedulerLF::SchedulerLF(uint32 mode) : m_mode(mode)  {
}

SchedulerLF::~SchedulerLF() {
}

bool SchedulerLF::init(ReactorPtr pReactor) {
    if (NULL == pReactor) {
        return false;
    }
    m_reactor = pReactor;
    m_reactorUnit = std::make_shared<SchdeulerUnit>([](){});
    m_work.store(true);
    return true;
}

void SchedulerLF::start() {
    m_start = true;
    m_startCond.notify_all();

    if ((m_mode & SCHEDULE_LF_MODE_REACTOR)) {
        UnitPtr pUnit = m_reactorUnit;
        m_workList.pushWithNotify(pUnit);
    }
}

void SchedulerLF::stop() {
    m_work.store(false);
    m_workList.notify(true);
}

void SchedulerLF::post(UnitPtr& ptr) {
    if (NULL == ptr) {
        return;
    }
    m_workList.pushWithNotify(ptr);
}

void SchedulerLF::run() {
    startWait();

    do {
        UnitPtr pUnit = m_workList.getTast();
        if (pUnit) {
            if (m_reactorUnit == pUnit) {
                reactorFunc();
            } else {
                pUnit->complete();
            }
        } else if (m_work.load()) {
            m_workList.wait();
        }
    } while(m_work.load());
}

void SchedulerLF::reactorFunc() {
    // reactor 粒度改为50ms
    m_reactor->reactorWait(m_taskList, 50);
    // 追加reactor切换任务
    m_taskList.push_back(m_reactorUnit);
    // 添加任务
    m_workList.pushBatchWithNotify(m_taskList);
}

void SchedulerLF::startWait() {
    std::unique_lock<std::mutex> lock(m_startMutex);
    m_startCond.wait(lock, [this](){ return this->m_start; });
}

NS_UTILS_END
