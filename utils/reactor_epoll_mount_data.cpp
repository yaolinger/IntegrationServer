#include "reactor_epoll_mount_data.hpp"

#include <vector>
#include "log.hpp"
#include "reactor_epoll.hpp"

NS_UTILS_BEGIN

// 初始化即可写
ReactorEpollMountData::ReactorEpollMountData(int32 fd, ReactorEpollPtr pRE) : m_curEvent(0), m_fd(fd), m_reactor(pRE) {
    m_tiggerEventMap[REACTOR_ACTIVATE_EVENT_WRITE] = true;
    m_cancelFlag.store(false);
}

bool ReactorEpollMountData::addUnit(UnitPtr ptr, REACTOR_ACTIVATE_EVENT event) {
    if (m_cancelFlag.load()) {
        log_warn("Socket[%d] aleady cancel all unit.", m_fd);
        return false;
    }

    if (NULL == ptr) {
        log_error("NULL == ptr");
        return false;
    }
    if (event == REACTOR_ACTIVATE_EVENT_ERROR) {
        log_error("REACTOR_ACTIVATE_EVENT_ERROR not need add.");
        return false;
    }
    std::lock_guard<std::mutex> lk(m_unitMutex);
    m_unitMap[event] = ptr;
    return true;
}

void ReactorEpollMountData::runEventOp(std::list<UnitPtr>& taskList, uint32 event) {
    if (m_cancelFlag.load()) {
        log_warn("Socket[%d] aleady cancel all unit");
        return;
    }

    // 添加触发标识
    {
        std::lock_guard<std::mutex> lk(m_triggerMutex);
        if (event & (REACTOR_ACTIVATE_EVENT_READ | REACTOR_ACTIVATE_EVENT_ERROR)) {
            m_tiggerEventMap[REACTOR_ACTIVATE_EVENT_READ] = true;
        } else if (event & (REACTOR_ACTIVATE_EVENT_WRITE | REACTOR_ACTIVATE_EVENT_ERROR)) {
            m_tiggerEventMap[REACTOR_ACTIVATE_EVENT_WRITE] = true;
        }
    }

    // 添加任务
    {
      std::lock_guard<std::mutex> lk(m_unitMutex);
      std::vector<uint32> runEventVec;
      for (auto& kv : m_unitMap) {
          if (event & (kv.first | REACTOR_ACTIVATE_EVENT_ERROR)) {
              taskList.push_back(kv.second);
              runEventVec.push_back(kv.first);
          }
      }
      for (auto& value : runEventVec) {
          m_unitMap.erase(value);
      }
    }
}

void ReactorEpollMountData::startEvent(REACTOR_EVENT event, UnitPtr pUnit) {
    if (m_reactor) {
        m_reactor->startEvent(event, shared_from_this(), pUnit);
    }
}

void ReactorEpollMountData::post(UnitPtr pUnit) {
    if (m_reactor) {
        m_reactor->post(pUnit);
    }
}

bool ReactorEpollMountData::getTiggerEvent(uint32 event) {
    std::lock_guard<std::mutex> lk(m_triggerMutex);
    auto iter = m_tiggerEventMap.find(event);
    if (iter != m_tiggerEventMap.end()) {
        return iter->second;
    }
    return false;
}

bool ReactorEpollMountData::runIOFunc(uint32 event, std::function<bool()> IOFunc) {
    // 注意这里return true
    if (m_cancelFlag.load()) {
        log_warn("Socket[%d] aleady cancel all unit");
        return true;
    }

    // IOFunc(accept, recv, send) 在锁范围内执行
    // 保证erase event 和 reactor 触发 add event 数据处理正确
    // 防止多线程异步流程触发标记覆盖
    std::lock_guard<std::mutex> lk(m_triggerMutex);
    if (!IOFunc()) {
        m_tiggerEventMap.erase(event);
        return false;
    }
    return true;
}

void ReactorEpollMountData::cancelUnits() {
    m_cancelFlag.store(true);
    // 清除标识
    {
        std::lock_guard<std::mutex> lk(m_triggerMutex);
        m_tiggerEventMap.clear();
    }
    // 清除待执行unit
    {
       std::lock_guard<std::mutex> lk(m_unitMutex);
       m_unitMap.clear();
    }
    if (m_reactor) {
        m_reactor->delEvent(m_fd);
    }
}

NS_UTILS_END
