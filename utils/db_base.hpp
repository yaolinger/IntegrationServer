#ifndef UTILS_DB_BASE_HPP
#define UTILS_DB_BASE_HPP

#include <map>

#include "macros.hpp"

NS_UTILS_BEGIN

// 链接池最小数量
#define DB_POOL_MIN_COUNT 5
// 链接超时最小时间
#define DB_CONN_MIN_TIMEOUT 5
// 错误长度
#define DB_ERROR_LEN 1000
// sql长度
#define SQL_MAX_LEN 500
// 错误信息
#define DB_EXEC_SUCCESS "db exec success"
#define DB_CONN_IS_NULL "db conn is null"
// 管理器最小线程数量
#define DB_MANAGER_THREAD_MIN_COUNT 1

// 数据库种类
enum DB_KIND {
    DB_KIND_MYSQL = 1,           // mysql
    DB_KIND_POSTGRESQL = 2,      // postgresql
    DB_KIND_SQLITE = 3,          // sqlite
    DB_KIND_ORACLE = 4,          // oracle
};

// 数据类型
enum DATA_TYPE {
    DATA_TYPE_UNKNOWN = 0,
    DATA_TYPE_STRING = 1,
    DATA_TYPE_INT = 2,
    DATA_TYPE_LLONG = 3,
    DATA_TYPE_UINT = 4,
    DATA_TYPE_ULLONG = 5,
    DATA_TYPE_DOUBLE = 6,
    DATA_TYPE_BLOB = 7,
    DATA_TYPE_TIMESTAMP = 8,
};

// 数据库初始化数据
struct DBInitData {
    DB_KIND Kind;               // 数据库种类
    uint32 connMaxCount;        // 链接最大数量
    uint32 connInitCount;       // 链接初始数量
    uint32 timeOut;             // 超时时间
    std::string host;           // 数据库地址
    std::string datebase;       // 数据库
    std::string user;           // 用户
    std::string password;       // 密码
};

// 数据库表信息
struct DBTableData {
    std::string table;         // 表名
    std::string where;         // 查询条件
    bool startRead;            // 启动加载
};

// 表结构缓存
struct DBTableCache {
    std::string table;                        // 表名
    std::string where;                        // 查询条件
    bool startRead;                           // 启动加载
    std::map<std::string, uint32> fieldMap;   // 字段(名字:数据类型)
};

NS_UTILS_END

#endif
