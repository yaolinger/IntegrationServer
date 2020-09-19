#ifndef UTILS_REACTOR_EVENT_HPP
#define UTILS_REACTOR_EVENT_HPP

#include <memory>
#include "macros.hpp"

NS_UTILS_BEGIN

// reactor 事件
enum REACTOR_EVENT {
    REACTOR_EVENT_READ = 0x01,    // 读事件
    REACTOR_EVENT_WRITE = 0x02,   // 写事件
    REACTOR_EVENT_ERROR = 0x04,   // 错误事件
};

// Socket数据
struct SocketData {
    int32 fd;                     // socket
    int32 curEvent;               // 当前epoll监听的事件
};
typedef std::shared_ptr<SocketData> SocketDataPtr;

// 反应堆事件处理
class ReactorEvent {
public:
    // 注册事件
    static void registerEventFunc();
    // 事件处理接口
    static void dealEvent(int32 fd, int32 type);
};

NS_UTILS_END

#endif
