#include "db_manager.hpp"
#include "log.hpp"

NS_UTILS_BEGIN

DBManager::DBManager()
    : m_scheduler(SCHEDULE_LF_MODE_TASK_LIST) {
}

bool DBManager::init(uint32 threadCount, const DBInitData& db, const std::vector<DBTableData>& tableVec) {
    bool ret = true;
    if (threadCount < DB_MANAGER_THREAD_MIN_COUNT) {
        threadCount = DB_MANAGER_THREAD_MIN_COUNT;
    }

    // 初始化链接池
    ret = m_dbPool.init(db.Kind, db.connMaxCount, db.connInitCount, db.timeOut, db.host, db.datebase, db.user, db.password);
    // 创建线程
    m_threadPool.createThread([this](){ m_scheduler.run(); }, threadCount);
    // 启动调度器
    m_scheduler.start();
    // 加载数据结构
    ret = loadTableField(tableVec);

    return ret;
}

void DBManager::stop() {
    m_scheduler.stop();
    m_threadPool.join();
}

bool DBManager::loadTableField(const std::vector<DBTableData>& tableVec) {
    bool ret = true;

    // 处理结果
    auto dealResult = [this, &ret] (const std::string& table, ResultSet_T result) {
        DBTableCache tableCache;
        tableCache.table = table;
        while(ResultSet_next(result)) {
            std::string fieldName = ResultSet_getString(result, 1);
            std::string fieldTypeStr = ResultSet_getString(result, 2);
            uint32 fieldType = DATA_TYPE_UNKNOWN;
            if (fieldTypeStr.find("bigint") != std::string::npos) {
                if (fieldTypeStr.find("unsigned") != std::string::npos) {
                    fieldType = DATA_TYPE_ULLONG;
                } else {
                    fieldType = DATA_TYPE_LLONG;
                }
            } else if (fieldTypeStr.find("int") != std::string::npos) {
                if (fieldTypeStr.find("unsigned") != std::string::npos) {
                    fieldType = DATA_TYPE_UINT;
                } else {
                    fieldType = DATA_TYPE_INT;
                }
            } else if (fieldTypeStr.find("blob") != std::string::npos) {
                fieldType = DATA_TYPE_BLOB;
            } else if (fieldTypeStr.find("varchar") != std::string::npos || fieldTypeStr.find("text") != std::string::npos) {
                fieldType = DATA_TYPE_STRING;
            } else {
                //TODO: 其他类型暂不处理
            }
            if (fieldType == DATA_TYPE_UNKNOWN) {
                ret = false;
                log_error("Table[%s] field[%s] type[%s] is unknown.", table.c_str(), fieldName.c_str(), fieldTypeStr.c_str());
                return ;
            }
            tableCache.fieldMap[fieldName] = fieldType;
        }
        m_tableCacheMap[table] = tableCache;
    };

    std::string error;
    for (const auto& value : tableVec) {
        m_dbPool.execQuery(error, std::bind(dealResult, value.table, std::placeholders::_1), "SHOW COLUMNS FROM %s;", value.table.c_str());
        if (error != DB_EXEC_SUCCESS) {
            log_error("Exec sql error[%s]", error.c_str());
            return false;
        }
        if (!ret) {
            return ret;
        }
        m_tableCacheMap[value.table].where = value.where;
        m_tableCacheMap[value.table].startRead = value.startRead;
    }

    return ret;
}

NS_UTILS_END
