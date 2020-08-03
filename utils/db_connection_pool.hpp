#ifndef UTILS_DB_CONNECTION_POOL_HPP
#define UTILS_DB_CONNECTION_POOL_HPP

#include <atomic>
#include <functional>
#include <vector>
#include <string>
#include <zdb.h>

#include "db_base.hpp"

NS_UTILS_BEGIN

class DBObserverBase;

// 数据库链接池
class DBConnectionPool {
friend class DBObserverBase;
public:
    DBConnectionPool();

    // 初始化
    bool init(DB_KIND kind, uint32 connMaxCount, uint32 initCount, uint32 timeout, const std::string& host, const std::string& db, const std::string& user, const std::string& password);

    // 执行语句(update, delete, insert 等无结果集的操作)
    // 带有结果集的操作, Connection_close(conn)后, 无法再次获得链接, 只能等待超时关闭
    void exec(const std::string& sql, std::string& error);

    // 执行查询语句(select等有结果集的操作)
    void execQuery(const std::string& sql, std::string& error, std::function<void(ResultSet_T result)> func);

    // 执行查询语句(select等有结果集的操作)
    void execQuery(const std::string& sql, const std::vector<std::pair<uint32, std::string>>& paramVec, std::string& error, std::function<void(ResultSet_T result)> func);

    // 执行外部函数(不可传入异步函数)
    void execFunc(std::string& error, std::function<void(Connection_T conn)> func);

    // 关闭链接池
    void close();

private:
    URL_T m_url;                 // 地址
    ConnectionPool_T m_pool;     // 链接池
};

// 数据库观察者基础(测试使用)
class DBObserverBase {
public:
    uint32 getSize(const DBConnectionPool& pool) { return ConnectionPool_size(pool.m_pool); }

    uint32 getActive(const DBConnectionPool& pool) { return ConnectionPool_active(pool.m_pool); }

    virtual void look(const DBConnectionPool& pool) = 0;
};

NS_UTILS_END

#endif
