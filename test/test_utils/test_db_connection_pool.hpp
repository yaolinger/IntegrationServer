#ifndef TEST_UTILS_TEST_DB_CONNECTION_POOL_HPP
#define TEST_UTILS_TEST_DB_CONNECTION_POOL_HPP

#include <chrono>
#include <thread>

#include "utils/db_connection_pool.hpp"
#include "utils/log.hpp"
#include "utils/log_helper.hpp"
#include "utils/macros.hpp"
#include "utils/thread_pool.hpp"

class DBObserver : public UTILS::DBObserverBase {
public:
    void look(const UTILS::DBConnectionPool& pool) {
        log_debug("DBConnectionPool size[%u] active size[%u]", UTILS::DBObserverBase::getSize(pool), UTILS::DBObserverBase::getActive(pool));
    }
};

void TestDBConnectionPool() {
    FUNC_TRACE;
    USING_NS_UTILS;

    DBConnectionPool dbPool;
    uint32 maxCount = 10;
    uint32 initCount = 2;
    uint32 timeout = 10;
    std::string host("0.0.0.0:3306");
    std::string db("test");
    std::string user("zjx");
    std::string password("1127406486");
    dbPool.init(DB_KIND_MYSQL, maxCount, initCount, timeout, host, db, user, password);

    DBObserver ob;
    ob.look(dbPool);
  //  std::this_thread::sleep_for(std::chrono::seconds(1));
  //  for (uint32 i = 0; i < maxCount; i++) {
  //      std::string error;
  //      std::string sql("insert into id_value (id, value) values(1, 'yin tree si ting')");
  //      dbPool.exec(sql, error);
  //      dbPool.exec(sql, error);
  //      dbPool.exec(sql, error);
  //   // dbPool.execQuery(sql, error, [](ResultSet_T result) {} );
  //   // dbPool.execFunc(error, [&ob, &dbPool](Connection_T conn){
  //   //          log_debug("ping[%d]", Connection_ping(conn));
  //   //          ob.look(dbPool); });
  //      if (error != DB_EXEC_SUCCESS) {
  //          log_error("result:%s", error.c_str());
  //      }
  //      ob.look(dbPool);

    // 多线程执行sql
    auto func = [&ob, &dbPool]() {
        std::string error;
        std::string sql("insert into id_value (id, value) values(1, 'yin tree si ting')");
        dbPool.exec(sql, error);
        ob.look(dbPool);
    };
    ThreadPool pool;
	pool.createThread(func, 10);
	pool.join();

    dbPool.close();
}

#endif
