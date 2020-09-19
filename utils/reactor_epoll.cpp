#include "reactor_epoll.hpp"

#include <unistd.h>
#include <fcntl.h>
#include <memory>
#include <sys/epoll.h>

#include "log.hpp"
#include "rand.hpp"
#include "reactor_event.hpp"
#include "scheduler_unit.hpp"

NS_UTILS_BEGIN

ReactorEpoll::ReactorEpoll() : m_epollFd(-1), m_timerFd(-1) {
}

bool ReactorEpoll::init() {
    m_epollFd = doEpollCreate();
    if (-1 == m_epollFd) {
        return false;
    }
    return true;
}

void ReactorEpoll::reactorWait(std::list<UnitPtr>& taskList, int32 timeout) {
    if (-1 == m_timerFd) {
        timeout = timeout < 0 ? s_maxTimeout : std::min(timeout, s_maxTimeout);
    }

    epoll_event events[s_eventSize];
    int numEvents = epoll_wait(m_epollFd, events, s_eventSize, timeout);

    // 事件处理
    for (int32 i = 0; i < numEvents; i++) {
        uint32 eventType = 0;
        if ((events[i].events & EPOLLIN) != 0) {
            eventType |= REACTOR_EVENT_READ;
        }
        if ((events[i].events & EPOLLOUT) != 0) {
            eventType |= REACTOR_EVENT_WRITE;
        }
        if ((events[i].events & (EPOLLERR | EPOLLHUP)) != 0) {
            eventType |= REACTOR_EVENT_ERROR;
        }
        // 计时器socket处理
        if (events[i].data.fd == m_timerFd) {
            log_info("TODO: timerfd[%d]暂时未实现", m_timerFd);
            continue;
        }
        // 投递事件
        UnitPtr pUnit = std::make_shared<SchdeulerUnit>(std::bind(ReactorEvent::dealEvent, (int32)events[i].data.fd, eventType));
        taskList.push_back(pUnit);
    }
}

bool ReactorEpoll::registerReadEvent(SocketDataPtr pSocket) {
    // pSocket 外部检测
    epoll_event ev;
    ev.events = EPOLLIN | EPOLLERR | EPOLLHUP | EPOLLPRI | EPOLLET;
    ev.data.fd = pSocket->fd;
    pSocket->curEvent = ev.events;
    int32 result = epoll_ctl(m_epollFd, EPOLL_CTL_ADD, ev.data.fd, &ev);
    if (0 != result) {
        m_error = "epoll register read event error[" + std::to_string(errno) + "]";
    }
    return 0 == result;
}

bool ReactorEpoll::registerWriteEvent(SocketDataPtr pSocket) {
    // pSocket 外部检测
    epoll_event ev;
    ev.events = EPOLLOUT | EPOLLERR | EPOLLHUP | EPOLLPRI | EPOLLET;
    ev.data.fd = pSocket->fd;
    pSocket->curEvent = ev.events;
    int32 result = epoll_ctl(m_epollFd, EPOLL_CTL_ADD, ev.data.fd, &ev);
    if (0 != result) {
        m_error = "epoll register read event error[" + std::to_string(errno) + "]";
    }
    return 0 == result;
}

bool ReactorEpoll::changeEvent(EPOLL_EVENT_OP op, SocketDataPtr pSocket) {
    // pSocket 外部检测
    if (0 == pSocket->curEvent) {
        // TODO::如果未注册事件如何报错?
    } else {
        int32 curEvent = pSocket->curEvent;
        if ((curEvent & EPOLLIN) == 0 && op == EPOLL_EVENT_OP_ADD_READ) {
            // 添加读事件
            curEvent |= EPOLLIN;
        } else if ((curEvent & EPOLLIN) == EPOLLIN && op == EPOLL_EVENT_OP_DEL_READ) {
            // 删除读事件
            curEvent ^= EPOLLIN;
        } else if ((curEvent & EPOLLOUT) == 0 && op == EPOLL_EVENT_OP_ADD_WRITE) {
            // 添加写事件
            curEvent |= EPOLLOUT;
        } else if ((curEvent & EPOLLOUT) == EPOLLOUT && op == EPOLL_EVENT_OP_DEL_WRITE) {
            // 删除写事件
            curEvent ^= EPOLLOUT;
        } else {
            log_warn("Socket[%d] curEvent[%d] op[%u] not change.", pSocket->fd, pSocket->curEvent, op);
            return true;
        }

        if ((curEvent & EPOLLIN) == 0 && (curEvent & EPOLLOUT) == 0) {
            // 既无读事件 也无写事件
            pSocket->curEvent = 0;
            epoll_event ev;
            ev.events = 0;
            ev.data.fd = pSocket->fd;
            epoll_ctl(m_epollFd, EPOLL_CTL_DEL, ev.data.fd, &ev);
        } else {
            // 修改监听事件
            epoll_event ev;
            ev.events = curEvent;
            ev.data.fd = pSocket->fd;
            if (epoll_ctl(m_epollFd, EPOLL_CTL_MOD, ev.data.fd, &ev) == 0) {
                pSocket->curEvent |= curEvent;
            } else {
                // TODO::如果处理错误
            }
        }

    }
    return true;
}

bool ReactorEpoll::delEvent(SocketDataPtr pSocket) {
    // pSocket 外部检测
    epoll_event ev;
    ev.events = 0;
    ev.data.fd = pSocket->fd;
    pSocket->curEvent = ev.events;
    int32 result = epoll_ctl(m_epollFd, EPOLL_CTL_DEL, ev.data.fd, &ev);
    if (0 != result) {
        m_error = "epoll del event error[" + std::to_string(errno) + "]";
    }
    return 0 == result;
}

int32 ReactorEpoll::doEpollCreate() {
    // linux 2.6.8 以上 epoll size 大于0即可
    int32 fd = epoll_create(s_epollSize);
    // 设置开启子进程关闭socket(个人不采用开启子进程调用方式)
    if (-1 != fd) {
        ::fcntl(fd, F_SETFD, FD_CLOEXEC);
    }
    return fd;
}

NS_UTILS_END
