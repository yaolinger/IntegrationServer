#ifndef UTILS_REACTOR_SOCKET_DATA_HPP
#define UTILS_REACTOR_SOCKET_DATA_HPP

#include <atomic>
#include <list>
#include <map>
#include <mutex>

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
    ReactorSocketData(int32 fd);
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
    // 获取触发事件标记
    bool getTiggerEvent(uint32 event);
    // 执行IOFunc
    // IOFunc: ture:执行成功, false:执行失败(EAGAIN)等待重新激活
    // return: true:执行ioFunc成功, false:执行ioFunc失败,需要重新发起异步io请求
    bool runIOFunc(uint32 event, std::function<bool()> IOFunc);
    // 取消Units
    void cancelUnits();

private:
    int32 m_fd;                                   // socket
    std::mutex m_triggerMutex;                    // 事件锁
    std::map<uint32, bool> m_tiggerEventMap;      // 事件触发map(event:bool)
    std::mutex m_unitMutex;                       // 操作锁
    std::map<uint32, UnitPtr> m_unitMap;          // 操作map (event:rUnit)
    std::atomic<bool> m_cancelFlag;               // 结束标识
};

// socket数据智能指针
typedef std::shared_ptr<ReactorSocketData> ReactorSocketDataPtr;

NS_UTILS_END

#endif
