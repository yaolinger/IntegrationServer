#include "reactor_epoll.hpp"

#include <unistd.h>
#include <fcntl.h>
#include <memory>
#include <sys/epoll.h>

#include "error.hpp"
#include "log.hpp"
#include "rand.hpp"
#include "scheduler_unit.hpp"

NS_UTILS_BEGIN

int32 ReactorEpoll::s_epollSize = 20000;           // epoll size
int32 ReactorEpoll::s_eventSize = 128;             // boost标准是128, 其他也有动态写法
int32 ReactorEpoll::s_maxTimeout = 5 * 60 * 1000;  // boost标准是5分钟

ReactorEpoll::ReactorEpoll() : m_epollFd(-1), m_timerFd(-1) {
}

ReactorEpoll::~ReactorEpoll() {
}

bool ReactorEpoll::init(SchedulerPtr pScheduler) {
    if (NULL == pScheduler) {
        return false;
    }
    m_scheduler = pScheduler;
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
        uint32 eventMask = 0;
        if (events[i].data.fd == m_timerFd) {
            log_warn("TODO: timerfd[%d]暂时未实现", m_timerFd);
            continue;
        }

        if ((events[i].events & EPOLLIN) != 0) {
            eventMask |= REACTOR_EVENT_READ;
        }
        if ((events[i].events & EPOLLOUT) != 0) {
            eventMask |= REACTOR_EVENT_WRITE;
        }
        if ((events[i].events & (EPOLLERR | EPOLLHUP)) != 0) {
            eventMask |= REACTOR_EVENT_ERROR;
        }
        ReactorSocketDataPtr pSocket = getReactorSocket((int32)events[i].data.fd);
        if (pSocket) {
            pSocket->runEventOp(taskList, eventMask);
        }
    }
}

bool ReactorEpoll::registerReadEvent(ReactorSocketDataPtr ptr, UnitPtr pUnit) {
    // ptr 外部检测
    epoll_event ev;
    ev.events = EPOLLIN | EPOLLERR | EPOLLHUP | EPOLLPRI | EPOLLET;
    ev.data.fd = ptr->getFd();
    if (0 == epoll_ctl(m_epollFd, EPOLL_CTL_ADD, ptr->getFd(), &ev)) {
        ptr->m_curEvent = ev.events;
        ptr->registerEventCallback(pUnit, REACTOR_EVENT_READ);
        m_socketMap[ptr->getFd()] = ptr;
        return true;
    } else {
        m_error = "Register read event error, " + GET_SYSTEM_ERRNO_INFO;
        return false;
    }
}

bool ReactorEpoll::addEvent(EPOLL_EVENT_OP op, ReactorSocketDataPtr ptr, UnitPtr pUnit) {
    if (EPOLL_EVENT_OP_READ == op) {
        if (ptr->getTiggerEvent(REACTOR_EVENT_READ)) {
            // 当前可读 直接执行unit
            m_scheduler->post(pUnit);
        } else {
            // 不可读添加unit待执行
            ptr->registerEventCallback(pUnit, REACTOR_EVENT_READ);
        }
    } else if (EPOLL_EVENT_OP_WRITE == op) {
        // 当前可写 直接执行unit
        if (ptr->getTiggerEvent(REACTOR_EVENT_WRITE)) {
            m_scheduler->post(pUnit);
            return true;
        }
        // 当前不可写 添加写事件及unit待执行
        if (!(ptr->m_curEvent & EPOLLOUT)) {
            epoll_event ev;
            ev.events = ptr->m_curEvent;
            ev.events |= EPOLLOUT;
            ev.data.fd = ptr->getFd();
            if (0 == epoll_ctl(m_epollFd, EPOLL_CTL_MOD, ptr->getFd(), &ev)) {
                ptr->m_curEvent = ev.events;
                ptr->registerEventCallback(pUnit, REACTOR_EVENT_WRITE);
            } else {
                return false;
            }
        } else {
            ptr->registerEventCallback(pUnit, REACTOR_EVENT_WRITE);
        }
    }
    return true;
}

bool ReactorEpoll::delEvent(int32 fd) {
    epoll_event ev;
    ev.events = 0;
    if (0 == epoll_ctl(m_epollFd, EPOLL_CTL_DEL, fd, &ev)) {
        m_socketMap.erase(fd);
        return true;
    } else {
        m_error = "Del socket event error, " + GET_SYSTEM_ERRNO_INFO;
        return false;
    }
}

void ReactorEpoll::post(UnitPtr pUnit) {
    if (m_scheduler) {
        m_scheduler->post(pUnit);
    }
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

ReactorSocketDataPtr ReactorEpoll::getReactorSocket(int32 fd) {
    auto iter = m_socketMap.find(fd);
    if (iter == m_socketMap.end()) {
        return NULL;
    }
    return iter->second;
}

NS_UTILS_END
