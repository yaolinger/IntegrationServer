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
    uint32 timeout = 5;
    std::string host("0.0.0.0:3306");
    std::string db("test");
    std::string user("zjx");
    std::string password("1127406486");
    dbPool.init(DB_KIND_MYSQL, maxCount, initCount, timeout, host, db, user, password);

    DBObserver ob;
    ob.look(dbPool);

    // 多线程执行sql
    auto func = [&ob, &dbPool]() {
        std::string error;
        dbPool.exec(error, "insert into id_value (id, value) values(1, 'yin tree si ting')");
        ob.look(dbPool);
    };
    ThreadPool pool;
	pool.createThread(func, 10);
	std::this_thread::sleep_for(std::chrono::seconds(10));
    ob.look(dbPool);
    pool.join();

    dbPool.close();
}

#endif
