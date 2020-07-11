#include "db_connection_pool.hpp"

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

void DBConnectionPool::exec(const std::string& sql, std::string& error) {
    Connection_T conn = ConnectionPool_getConnection(m_pool);
    if (NULL == conn) {
        error = DB_CONN_IS_NULL;
        return;
    }

    TRY
    {
        Connection_execute(conn, "%s", sql.c_str());
        error = DB_EXEC_SUCCESS;
    }
    CATCH(SQLException)
    {
        char buf[DB_ERROR_LEN] = {0};
        snprintf(buf, sizeof(buf), "exec sql error:%s", Exception_frame.message);
        error = buf;
    }
    END_TRY;
    Connection_close(conn);
}

void DBConnectionPool::execQuery(const std::string& sql, std::string& error, std::function<void(ResultSet_T result)> func) {
    Connection_T conn = ConnectionPool_getConnection(m_pool);
    if (NULL == conn) {
        error = DB_CONN_IS_NULL;
        return;
    }

    TRY
    {
        ResultSet_T result = Connection_executeQuery(conn, "%s", sql.c_str());
        func(result);
        error = DB_EXEC_SUCCESS;
    }
    CATCH(SQLException)
    {
        char buf[DB_ERROR_LEN] = {0};
        snprintf(buf, sizeof(buf), "exec sql error:%s", Exception_frame.message);
        error = buf;
    }
    END_TRY;
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
        snprintf(buf, sizeof(buf), "exec sql error:%s", Exception_frame.message);
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
