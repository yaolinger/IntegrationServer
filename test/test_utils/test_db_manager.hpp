#ifndef TEST_UTILS_TEST_DB_MANAGER_HPP
#define TEST_UTILS_TEST_DB_MANAGER_HPP

#include <vector>

#include "utils/db_manager.hpp"
#include "utils/log.hpp"
#include "utils/log_helper.hpp"
#include "utils/macros.hpp"

std::vector<UTILS::DBTableData> tableVec = {{"role", "rk=?", true}};
UTILS::DBInitData dbInit = { UTILS::DB_KIND_MYSQL, 10, 2, 10, "0.0.0.0:3306", "test", "zjx", "1127406486" };
void TestDBManager() {
    FUNC_TRACE;
    USING_NS_UTILS;
    uint32 threadCount = 3;
    DBManager dbMgr;
    dbMgr.init(threadCount, dbInit, tableVec);
    dbMgr.stop();
}

#endif
