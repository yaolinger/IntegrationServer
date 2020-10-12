#ifndef UTILS_REACTOR_EPOLL_HPP
#define UTILS_REACTOR_EPOLL_HPP

#include "reactor_epoll_mount_data.hpp"
#include "scheduler_base.hpp"

NS_UTILS_BEGIN

// 基于epoll的reactor实现
class ReactorEpoll : public ReactorBase {
public:
    ReactorEpoll();
    ~ReactorEpoll();

public:
    // 初始化
    bool init(SchedulerPtr pScheduler);
    // 执行
    void reactorWait(std::list<UnitPtr>& taskList, int32 timeout);
    // 获取错误信息
    const std::string& getError() { return m_error; }
    // 开始监听事件
    bool startEvent(REACTOR_EVENT event, ReactorEpollMountDataPtr ptr, UnitPtr pUnit);
    // 移除事件
    bool delEvent(int32 fd);
    // 分发事件
    void post(UnitPtr pUnit);
    // 设置计时器检测
    void setTimerCheck(GetTimerTaskFunc func) { m_timerTaskFunc = func; }
private:
    // 创建epoll socket;
    static int32 doEpollCreate();
    // 获取mountData
    ReactorEpollMountDataPtr getMount(int32 fd);

private:
    int32 m_epollFd;                                          // epoll socket
    int32 m_timerFd;                                          // 计时器fd(暂时未实现)
    std::string m_error;                                      // 错误信息
    SchedulerPtr m_scheduler;                                 // 调度器
    std::map<int32, ReactorEpollMountDataPtr> m_mountMap;     // 挂载数据
    GetTimerTaskFunc m_timerTaskFunc;                         // 计时器任务Func

private:
    static int32 s_epollSize;   // epoll size
    static int32 s_eventSize;   // boost标准是128, 其他也有动态写法
    static int32 s_maxTimeout;  // boost标准是5分钟
};
// 智能指针
typedef std::shared_ptr<ReactorEpoll> ReactorEpollPtr;

NS_UTILS_END

#endif
