#ifndef SINGLE_SERVER_SERVER_GLOBAL_HPP
#define SINGLE_SERVER_SERVER_GLOBAL_HPP

// 活跃时间
#define LINK_ACTIVE_TIME 10
// 超时时间
#define LINK_TIMEOUT 15

class SingleServer;

struct global {
    static SingleServer* pServer;
};

#endif
