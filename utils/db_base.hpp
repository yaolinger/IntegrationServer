#ifndef UTILS_DB_BASE_HPP
#define UTILS_DB_BASE_HPP

#include "macros.hpp"

NS_UTILS_BEGIN

// 链接池最小数量
#define DB_POOL_MIN_COUNT 5
// 链接超时最小时间
#define DB_CONN_MIN_TIMEOUT 5
// 错误长度
#define DB_ERROR_LEN  1000
// 错误信息
#define DB_EXEC_SUCCESS "db exec success"
#define DB_CONN_IS_NULL "db conn is null"

// 数据库种类
enum DB_KIND {
    DB_KIND_MYSQL = 1,           // mysql
    DB_KIND_POSTGRESQL = 2,      // postgresql
    DB_KIND_SQLITE = 3,          // sqlite
    DB_KIND_ORACLE = 4,          // oracle
};

// 数据类型
enum DATA_TYPE {
    DATE_TYPE_STRING = 1,
    DATA_TYPE_INT = 2,
    DATA_TYPE_LLONG = 3,
    DATA_TYPE_DOUBLE = 4,
    DATA_TYPE_BLOB = 5,
    DATA_TYPE_TIMESTAMP = 6,
};

NS_UTILS_END

#endif
