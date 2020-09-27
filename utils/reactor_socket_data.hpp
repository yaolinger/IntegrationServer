#ifndef UTILS_REACTOR_SOCKET_DATA_HPP
#define UTILS_REACTOR_SOCKET_DATA_HPP

#include <list>
#include <map>

#include "macros.hpp"
#include "reactor_unit.hpp"

NS_UTILS_BEGIN

// reactor 事件
enum REACTOR_EVENT {
    REACTOR_EVENT_READ = 0x01,    // 读事件
    REACTOR_EVENT_WRITE = 0x02,   // 写事件
    REACTOR_EVENT_ERROR = 0x04,   // 错误事件
};

// reactor 挂载数据
class ReactorSocketData {
public:
    ReactorSocketData(int32 fd) : m_curEvent(0), m_fd(fd) {}
    ~ReactorSocketData() {}

public:
    // 注册事件执行单元
    bool registerEventCallback(UnitPtr ptr, REACTOR_EVENT event);
    // 执行事件
    void runEventOp(std::list<UnitPtr>& taskList, uint32 event);

public:
    // 获取socket
    int32 getFd() { return m_fd; }
    // 当前监听事件(写接口修改很麻烦)
    int32 m_curEvent;

private:
    int32 m_fd;                                   // socket
    std::map<uint32, UnitPtr> m_unitMap;          // 操作map (event:rUnit)
};

// socket数据智能指针
typedef std::shared_ptr<ReactorSocketData> ReactorSocketDataPtr;

NS_UTILS_END

#endif
