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
    bool init();
    // 执行
    void reactorWait(std::list<UnitPtr>& taskList, int32 timeOut);
    // 获取错误信息
    const std::string& getError() { return m_error; }
    // 注册读事件
    bool registerReadEvent(ReactorSocketDataPtr ptr, ReactorUnitPtr pUnit);
    // 增加事件
    bool addEvent(EPOLL_EVENT_OP op, ReactorSocketDataPtr ptr, ReactorUnitPtr pUnit);
    // 移除事件
    bool delEvent(int32 fd);

private:
    // 创建epoll socket;
    static int32 doEpollCreate();
    // 获取socketData
    ReactorSocketDataPtr getReactorSocket(int32 fd);

private:
    int32 m_epollFd;                                       // epoll socket
    int32 m_timerFd;                                       // 计时器fd(暂时未实现)
    std::string m_error;                                   // 错误信息
    std::map<int32, ReactorSocketDataPtr> m_socketMap;     // 挂载数据

private:
    static const int32 s_epollSize = 20000;           // epoll size
    static const int32 s_eventSize = 128;             // boost标准是128, 其他也有动态写法
    static const int32 s_maxTimeout = 5 * 60 * 1000;  // boost标准是5分钟
};

NS_UTILS_END

#endif
