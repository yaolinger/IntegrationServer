#include "db_connection_pool.hpp"

#include <stdarg.h>

#include "calc_tool.hpp"
#include "log.hpp"

NS_UTILS_BEGIN

DBConnectionPool::DBConnectionPool() : m_url(NULL), m_pool(NULL) {
}

bool DBConnectionPool::init(DB_KIND kind, uint32 connMaxCount, uint32 initCount, uint32 timeout, const std::string& host, const std::string& db, const std::string& user, const std::string& password) {
    std::string dbKind;
    if (DB_KIND_MYSQL == kind) {
        dbKind = "mysql";
    } else if (DB_KIND_ORACLE == kind) {
        dbKind = "oracle";
    } else if (DB_KIND_SQLITE == kind) {
        dbKind = "sqlite";
    } else if (DB_KIND_POSTGRESQL == kind) {
        dbKind = "postgresql";
    } else {
        log_error("DB kind[%u] not exist", kind);
        return false;
    }

    char buf[256] = {0};
    snprintf(buf, sizeof(buf), "%s://%s/%s?user=%s&password=%s&charset=utf8mb4", dbKind.c_str(), host.c_str(), db.c_str(), user.c_str(), password.c_str());
    m_url = URL_new(buf);
    if (NULL == m_url) {
        log_error("NULL == m_url, buf[%s]", buf);
        return false;
    }

    m_pool = ConnectionPool_new(m_url);
    if (NULL == m_pool) {
        log_error("NULL == m_pool");
        URL_free(&m_url);
        return false;
    }

    if (timeout < DB_CONN_MIN_TIMEOUT) {
        timeout = DB_CONN_MIN_TIMEOUT;
    }
    if (connMaxCount < DB_POOL_MIN_COUNT) {
        connMaxCount = DB_POOL_MIN_COUNT;
    }
    if (initCount > connMaxCount) {
        initCount = connMaxCount;
    }
    ConnectionPool_setInitialConnections(m_pool, initCount);
    ConnectionPool_setMaxConnections(m_pool, connMaxCount);
    ConnectionPool_setConnectionTimeout(m_pool, timeout);
    ConnectionPool_setReaper(m_pool, initCount);
    ConnectionPool_setAbortHandler(m_pool, [](const char* error){ log_error("Connection pool error:%s", error); });
    ConnectionPool_start(m_pool);

    log_info("Db[%s:%s:%s] connection pool init success, init count[%u] max count[%u].", dbKind.c_str(), host.c_str(), db.c_str(), initCount, connMaxCount);
    return true;
}

void DBConnectionPool::exec(std::string& error, const char* format, ...) {
    Connection_T conn = ConnectionPool_getConnection(m_pool);
    if (NULL == conn) {
        error = DB_CONN_IS_NULL;
        return;
    }
    char sql[SQL_MAX_LEN] = {0};
    va_list args;
    va_start(args, format);
    std::vsnprintf(sql, SQL_MAX_LEN - 1, format, args);
	va_end(args);

    TRY
    {
        Connection_execute(conn, "%s", sql);
        error = DB_EXEC_SUCCESS;
    }
    CATCH(SQLException)
    {
        char buf[DB_ERROR_LEN] = {0};
        snprintf(buf, sizeof(buf), "Sql[%s] error:[%s]", sql, Exception_frame.message);
        error = buf;
    }
    END_TRY;
	va_end(args);
    Connection_close(conn);
}

void DBConnectionPool::execQuery(std::string& error, std::function<void(ResultSet_T result)> func, const char* format, ...) {
    Connection_T conn = ConnectionPool_getConnection(m_pool);
    if (NULL == conn) {
        error = DB_CONN_IS_NULL;
        return;
    }
    char sql[SQL_MAX_LEN] = {0};
    va_list args;
    va_start(args, format);
    std::vsnprintf(sql, SQL_MAX_LEN - 1, format, args);
	va_end(args);

    TRY
    {
        ResultSet_T result = Connection_executeQuery(conn, "%s", sql);
        func(result);
        error = DB_EXEC_SUCCESS;
    }
    CATCH(SQLException)
    {
        char buf[DB_ERROR_LEN] = {0};
        snprintf(buf, sizeof(buf), "Sql[%s] error:[%s]", sql, Exception_frame.message);
        error = buf;
    }
    END_TRY;
	va_end(args);
    Connection_close(conn);
}

void DBConnectionPool::execQuery(std::string& error, std::function<void(ResultSet_T result)> func, const std::vector<std::pair<uint32, std::string>>& paramVec, const char* format, ...) {
    Connection_T conn = ConnectionPool_getConnection(m_pool);
    if (NULL == conn) {
        error = DB_CONN_IS_NULL;
        return;
    }
    char sql[SQL_MAX_LEN] = {0};
    va_list args;
    va_start(args, format);
    std::vsnprintf(sql, SQL_MAX_LEN - 1, format, args);
	va_end(args);

    TRY
    {
        PreparedStatement_T ps = Connection_prepareStatement(conn, "%s", sql);
        for (uint32 i = 0; i < paramVec.size(); i++) {
            switch (paramVec.at(i).first) {
                case DATA_TYPE_STRING:
                    PreparedStatement_setString(ps, i+1, paramVec.at(i).second.c_str());
                    break;
                case DATA_TYPE_INT:
                    PreparedStatement_setInt(ps, i+1, string_to_int32(paramVec.at(i).second));
                    break;
                case DATA_TYPE_LLONG:
                    PreparedStatement_setLLong(ps, i+1, string_to_int64(paramVec.at(i).second));
                    break;
                case DATA_TYPE_UINT:
                    PreparedStatement_setInt(ps, i+1, string_to_uint32(paramVec.at(i).second));
                    break;
                case DATA_TYPE_ULLONG:
                    PreparedStatement_setLLong(ps, i+1, string_to_uint64(paramVec.at(i).second));
                    break;
                case DATA_TYPE_DOUBLE:
                    // TODO:暂时不需要double类型
                    break;
                case DATA_TYPE_TIMESTAMP:
                    PreparedStatement_setTimestamp(ps, i+1, string_to_uint32(paramVec.at(i).second));
                    break;
                case DATA_TYPE_BLOB:
                    PreparedStatement_setBlob(ps, i+1, paramVec.at(i).second.c_str(),  paramVec.at(i).second.size());
                    break;
            }
        }

        ResultSet_T result = PreparedStatement_executeQuery(ps);
        func(result);
        error = DB_EXEC_SUCCESS;
    }
    CATCH(SQLException)
    {
        char buf[DB_ERROR_LEN] = {0};
        snprintf(buf, sizeof(buf), "Sql[%s] error:[%s]", sql, Exception_frame.message);
        error = buf;
    }
    END_TRY;
	va_end(args);
    Connection_close(conn);

}

void DBConnectionPool::execFunc(std::string& error, std::function<void(Connection_T conn)> func) {
    Connection_T conn = ConnectionPool_getConnection(m_pool);
    if (NULL == conn) {
        error = DB_CONN_IS_NULL;
        return;
    }

    TRY
    {
        func(conn);
        error = DB_EXEC_SUCCESS;
    }
    CATCH(SQLException)
    {
        char buf[DB_ERROR_LEN] = {0};
        snprintf(buf, sizeof(buf), "Exec sql error:%s", Exception_frame.message);
        error = buf;
    }
    END_TRY;
    Connection_close(conn);
}

void DBConnectionPool::close() {
    if (NULL != m_pool) {
        ConnectionPool_stop(m_pool);
        ConnectionPool_free(&m_pool);
    }
    if (NULL != m_url) {
        URL_free(&m_url);
    }
}

NS_UTILS_END
