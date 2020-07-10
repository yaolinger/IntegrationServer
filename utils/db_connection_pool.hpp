#ifndef UTILS_DB_CONNECTION_POOL_HPP
#define UTILS_DB_CONNECTION_POOL_HPP

#include <atomic>
#include <functional>
#include <string>
#include <zdb.h>

#include "macros.hpp"

NS_UTILS_BEGIN

// 链接池最小数量
#define DB_POOL_MIN_COUNT 5
// 链接超时最小时间
#define DB_CONN_MIN_TIMEOUT 5
// 错误信息
#define DB_ERROR_LEN  1000
#define DB_EXEC_SUCCESS "db exec success"
#define DB_CONN_IS_NULL "db conn is null"

// 数据库种类
enum DB_KIND {
    DB_KIND_MYSQL = 1,           // mysql
    DB_KIND_POSTGRESQL = 2,      // postgresql
    DB_KIND_SQLITE = 3,          // sqlite
    DB_KIND_ORACLE = 4,          // oracle
};

// 数据库链接池
class DBConnectionPool {
public:
    DBConnectionPool();

    // 初始化
    bool init(DB_KIND kind, uint32 connMaxCount, uint32 initCount, uint32 timeout, const std::string& host, const std::string& db, const std::string& user, const std::string& password);

    // 执行语句
    void exec(const std::string& sql, std::string& error);

    // 执行查询语句
    void execQuery(const std::string& sql, std::string& error, std::function<void(ResultSet_T result)> func);

    // 执行外部函数(不可包裹异步函数)
    void execFunc(std::string& error, std::function<void(Connection_T conn)> func);

    // 关闭链接池
    void close();

private:
    URL_T m_url;                 // 地址
    ConnectionPool_T m_pool;     // 链接池
};

NS_UTILS_END

#endif
