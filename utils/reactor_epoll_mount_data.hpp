#ifndef UTILS_REACTOR_EPOLL_MOUNT_DATA_HPP
#define UTILS_REACTOR_EPOLL_MOUNT_DATA_HPP

#include <atomic>
#include <list>
#include <map>
#include <mutex>

#include "macros.hpp"
#include "reactor_unit.hpp"

NS_UTILS_BEGIN

// 声明
class ReactorEpoll;
typedef std::shared_ptr<ReactorEpoll> ReactorEpollPtr;

// reactor epoll 挂载数据
class ReactorEpollMountData : public std::enable_shared_from_this<ReactorEpollMountData> {
public:
    ReactorEpollMountData(int32 fd, ReactorEpollPtr pRE);
    ~ReactorEpollMountData() {}

public:
    // 注册事件执行单元
    bool addUnit(UnitPtr ptr, REACTOR_ACTIVATE_EVENT event);
    // 执行事件
    void runEventOp(std::list<UnitPtr>& taskList, uint32 event);
    // 设置操作
    void startEvent(REACTOR_EVENT event, UnitPtr pUnit);
    // 提交任务
    void post(UnitPtr pUnit);
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
    ReactorEpollPtr m_reactor;                    // reactor
};

// socket数据智能指针
typedef std::shared_ptr<ReactorEpollMountData> ReactorEpollMountDataPtr;

NS_UTILS_END

#endif
