#include "reactor_socket_data.hpp"

#include <vector>
#include "log.hpp"

NS_UTILS_BEGIN

// 初始化即可写
ReactorSocketData::ReactorSocketData(int32 fd) : m_curEvent(0), m_fd(fd) {
    m_tiggerEventMap[REACTOR_EVENT_WRITE] = true;
    m_cancelFlag.store(false);
}

bool ReactorSocketData::registerEventCallback(UnitPtr ptr, REACTOR_EVENT event) {
    if (m_cancelFlag.load()) {
        log_warn("Socket[%d] aleady cancel all unit");
        return false;
    }

    if (NULL == ptr) {
        log_error("NULL == ptr");
        return false;
    }
    if (event == REACTOR_EVENT_ERROR) {
        log_error("REACTOR_EVENT_ERROR not need register");
        return false;
    }
    std::lock_guard<std::mutex> lk(m_unitMutex);
    m_unitMap[event] = ptr;
    return true;
}

void ReactorSocketData::runEventOp(std::list<UnitPtr>& taskList, uint32 event) {
    if (m_cancelFlag.load()) {
        log_warn("Socket[%d] aleady cancel all unit");
        return;
    }

    // 添加触发标识
    {
        std::lock_guard<std::mutex> lk(m_triggerMutex);
        if (event & (REACTOR_EVENT_READ | REACTOR_EVENT_ERROR)) {
            m_tiggerEventMap[REACTOR_EVENT_READ] = true;
        } else if (event & (REACTOR_EVENT_WRITE | REACTOR_EVENT_ERROR)) {
            m_tiggerEventMap[REACTOR_EVENT_WRITE] = true;
        }
    }

    // 添加任务
    {
      std::lock_guard<std::mutex> lk(m_unitMutex);
      std::vector<uint32> runEventVec;
      for (auto& kv : m_unitMap) {
          if (event & (kv.first | REACTOR_EVENT_ERROR)) {
              taskList.push_back(kv.second);
              runEventVec.push_back(kv.first);
          }
      }
      for (auto& value : runEventVec) {
          m_unitMap.erase(value);
      }
    }
}

bool ReactorSocketData::getTiggerEvent(uint32 event) {
    std::lock_guard<std::mutex> lk(m_triggerMutex);
    auto iter = m_tiggerEventMap.find(event);
    if (iter != m_tiggerEventMap.end()) {
        return iter->second;
    }
    return false;
}

bool ReactorSocketData::runIOFunc(uint32 event, std::function<bool()> IOFunc) {
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

void ReactorSocketData::cancelUnits() {
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
}

NS_UTILS_END
