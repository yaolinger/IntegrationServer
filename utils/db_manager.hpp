#ifndef UTILS_DB_MANAGER_HPP
#define UTILS_DB_MANAGER_HPP

#include <map>
#include <vector>
#include "db_base.hpp"
#include "db_connection_pool.hpp"
#include "macros.hpp"
#include "scheduler_leader_follower.hpp"
#include "sql.hpp"
#include "thread_pool.hpp"

NS_UTILS_BEGIN

// 表结构缓存
struct DBTableCache {
    std::string table;                        // 表名
    std::string where;                        // 查询条件
    SQLTablePtr sqlPtr;                       // sql表
};

// 数据库管理器
class DBManager {
public:
    DBManager();
    ~DBManager() {}

    // 初始化
    bool init(uint32 threadCount, const DBInitData& db, const std::vector<DBTableData>& tableVec);

    // 停止
    void stop();

    // 提交任务
    void postDBTask(UnitPtr& ptr) { m_scheduler.post(ptr); }

public:
    // 获取sql
    SQLTablePtr getTableSql(const char* table);
    // 获取链接池
    DBConnectionPool& getDBConnPool() { return m_dbPool; }
    // 获取调度器(用于构建SchdelerSafeUnit)
    SchedulerLF& getScheduler() { return m_scheduler; }

private:
    // 加载表结构
    bool loadTableField(const std::vector<DBTableData>& tableVec);
private:
    DBConnectionPool m_dbPool; // 数据库链接池
    SchedulerLF m_scheduler;   // 调度器(消息队列模式)
    ThreadPool m_threadPool;   // 线程池

private:
    std::map<std::string, DBTableCache> m_tableCacheMap;        // 表结构缓存
};

NS_UTILS_END

#endif
