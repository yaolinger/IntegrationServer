#ifndef SERVICE_CORE_SERVER_BASE_HPP
#define SERVICE_CORE_SERVER_BASE_HPP

#include "utils/macros.hpp"

NS_SERVICE_CORE_BEGIN

/* 服务对象3层
 * 1. Class ServerBase;
 * 2. Class PlanServer : ServerBase;
 * 3. Class LogicServer : PlanServer;
 */

// 服务基础
// 单一进程只允许一个server实例存在
class ServerBase {
public:
    ServerBase();

    ~ServerBase();

public:
    static ServerBase* getInstanice();

// 待实现(逻辑层)
public:
    // 服务名称
    virtual const char* getServerName() = 0;
    // 初始化服务
    virtual bool onInitServer() = 0;
    // 服务开始运行
    virtual void onRunServer() = 0;
    // 关闭服务
    virtual bool onCloseServer() = 0;

// 待实现(方案层)
public:
    // 初始化服务
    virtual bool initServer() = 0;
    // 运行主体
    virtual bool runServer() = 0;
    // 关闭服务
    virtual bool closeServer() = 0;
    // 信号处理
    virtual bool initSignal() = 0;

private:
    static ServerBase* m_server; // 服务对象
};

NS_SERVICE_CORE_END

#endif
