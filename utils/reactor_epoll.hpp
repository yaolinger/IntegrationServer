#ifndef UTILS_REACTOR_EPOLL_HPP
#define UTILS_REACTOR_EPOLL_HPP

#include "reactor_socket_data.hpp"
#include "scheduler_base.hpp"

NS_UTILS_BEGIN

// epoll事件动作
enum EPOLL_EVENT_OP {
    EPOLL_EVENT_OP_READ = 1,   // 添加读事件
    EPOLL_EVENT_OP_WRITE = 2,  // 添加写事件
};

// 基于epoll的reactor实现
class ReactorEpoll : public ReactorBase {
public:
    ReactorEpoll();
    ~ReactorEpoll();

public:
    // 初始化
    bool init(SchedulerPtr pScheduler);
    // 执行
    void reactorWait(std::list<UnitPtr>& taskList, int32 timeOut);
    // 获取错误信息
    const std::string& getError() { return m_error; }
    // 注册读事件
    bool registerReadEvent(ReactorSocketDataPtr ptr, UnitPtr pUnit);
    // 增加事件
    bool addEvent(EPOLL_EVENT_OP op, ReactorSocketDataPtr ptr, UnitPtr pUnit);
    // 移除事件
    bool delEvent(int32 fd);
    // 分发事件
    void post(UnitPtr pUnit);

private:
    // 创建epoll socket;
    static int32 doEpollCreate();
    // 获取socketData
    ReactorSocketDataPtr getReactorSocket(int32 fd);

private:
    int32 m_epollFd;                                       // epoll socket
    int32 m_timerFd;                                       // 计时器fd(暂时未实现)
    std::string m_error;                                   // 错误信息
    SchedulerPtr m_scheduler;                              // 调度器
    std::map<int32, ReactorSocketDataPtr> m_socketMap;     // 挂载数据

private:
    static int32 s_epollSize;   // epoll size
    static int32 s_eventSize;   // boost标准是128, 其他也有动态写法
    static int32 s_maxTimeout;  // boost标准是5分钟
};
// 智能指针
typedef std::shared_ptr<ReactorEpoll> ReactorEpollPtr;

NS_UTILS_END

#endif
