#ifndef TEST_UTILS_TEST_DB_MANAGER_HPP
#define TEST_UTILS_TEST_DB_MANAGER_HPP

#include <vector>

#include "utils/db_manager.hpp"
#include "utils/log.hpp"
#include "utils/log_helper.hpp"
#include "utils/macros.hpp"
#include "utils/scheduler.hpp"
#include "utils/scheduler_unit.hpp"
#include "utils/scheduler_safe_unit.hpp"

// 表数据
std::vector<UTILS::DBTableData> tableVec = {{"global_user", ""}};
static std::string s_testTable = "global_user";
// 数据库数据
UTILS::DBInitData dbInit = { UTILS::DB_KIND_MYSQL, 10, 2, 10, "0.0.0.0:3306", "test", "zjx", "1127406486" };

void TestDBManager() {
    FUNC_TRACE;
    USING_NS_UTILS;
    // 主线程调度器
    SchedulerLF masterScheduler(SCHEDULE_LF_MODE_TASK_LIST);
    masterScheduler.start();
    // 初始化数据库
    uint32 threadCount = 3;
    DBManager dbMgr;
    dbMgr.init(threadCount, dbInit, tableVec);
    // 关闭主线程
    UnitPtr pCloseMaster = std::make_shared<SchdeulerUnit>([&dbMgr, &masterScheduler]() {
        dbMgr.stop();
        masterScheduler.stop();
        log_warn("Logic thread close.");
    });

    // 测试任务提交
    // 构建线程安全执行单元
    std::shared_ptr<SchdelerSafeUnit> ssup = std::make_shared<SchdelerSafeUnit>(dbMgr.getScheduler());
    do {
        auto getTableCount = [](ResultSet_T result) {
            while(ResultSet_next(result)) {
                uint32 num =  ResultSet_getIntByName(result, "num");
                log_info("Role table will load data rows[%u]", num);
                break;
            }
        };
        auto dealResult = [](ResultSet_T result) {
            while(ResultSet_next(result)) {
                uint64 rk = ResultSet_getLLongByName(result, "rk");
                std::string name = ResultSet_getStringByName(result, "name");
                uint32 createTime = ResultSet_getIntByName(result, "create_time");
                log_info("Load role[%lu] name[%s] createTime[%u]", rk, name.c_str(), createTime);
            }
        };
        SQLTablePtr pSql = dbMgr.getTableSql(s_testTable.c_str());
        if (NULL == pSql) {
            log_error("NULL == pSql on table[%s]", s_testTable.c_str());
            break;
        }

        auto runSqlCount = [&dbMgr, &pSql, &getTableCount]() {
                    std::string error;
                    dbMgr.getDBConnPool().execQuery(error, getTableCount, pSql->selectCount().c_str());
                    if (error != DB_EXEC_SUCCESS) {
                        log_error("Exec sql error:%s", error.c_str());
                    }
        };

        auto runSql = [&dbMgr, &pSql, &dealResult]() {
                    std::string error;
                    dbMgr.getDBConnPool().execQuery(error, dealResult, pSql->selectAll().c_str());
                    if (error != DB_EXEC_SUCCESS) {
                        log_error("Exec sql error:%s", error.c_str());
                    }
        };

        // 非线程安全方式
        UnitPtr pUnit = std::make_shared<SchdeulerUnit>(runSqlCount);
        dbMgr.postDBTask(pUnit);

        // 线程安全
        ssup->post(runSql);
        ssup->post([&masterScheduler, &pCloseMaster]() { masterScheduler.post(pCloseMaster); } );
    } while(0);
    masterScheduler.run();
}

#endif
