#ifndef UTILS_DB_MANAGER_HPP
#define UTILS_DB_MANAGER_HPP

#include <map>
#include <vector>
#include "db_base.hpp"
#include "db_connection_pool.hpp"
#include "macros.hpp"
#include "scheduler_leader_follower.hpp"
#include "thread_pool.hpp"

NS_UTILS_BEGIN

// 数据库管理器
class DBManager {
public:
    DBManager();
    ~DBManager() {}

    // 初始化
    bool init(uint32 threadCount, const DBInitData& db, const std::vector<DBTableData>& tableVec);

    // 停止
    void stop();

private:
    // 加载表结构
    bool loadTableField(const std::vector<DBTableData>& tableVec);

private:
    DBConnectionPool m_dbPool; // 数据库链接池
    SchedulerLF m_scheduler;   // 调度器(消息队列模式)
    ThreadPool m_threadPool;   // 线程池

private:
    std::map<std::string, DBTableCache> m_tableCacheMap;       // 表结构缓存

};

NS_UTILS_END

#endif
