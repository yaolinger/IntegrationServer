#include "reactor_socket_data.hpp"

#include <vector>
#include "log.hpp"

NS_UTILS_BEGIN

bool ReactorSocketData::registerEventCallback(UnitPtr ptr, REACTOR_EVENT event) {
    if (NULL == ptr) {
        log_error("NULL == ptr");
        return false;
    }
    if (event == REACTOR_EVENT_ERROR) {
        log_error("REACTOR_EVENT_ERROR not need register");
        return false;
    }
    m_unitMap[event] = ptr;
    return true;
}

void ReactorSocketData::runEventOp(std::list<UnitPtr>& taskList, uint32 event) {
    // 查询事件 添加任务
    std::vector<uint32> runEventVec;
    for (auto& kv : m_unitMap) {
        if (event & (kv.first | REACTOR_EVENT_ERROR)) {
            taskList.push_back(kv.second);
            runEventVec.push_back(kv.first);
        }
    }

    // 删除监听任务
    for (auto& value : runEventVec) {
        m_unitMap.erase(value);
    }
}

NS_UTILS_END
