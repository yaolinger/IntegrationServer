#include "utils/log.hpp"
#include "utils/time_helper.hpp"
#include "test_logic_work/test_battle.hpp"
#include "test_syntax/test_cast.hpp"
#include "test_syntax/test_class.hpp"
#include "test_syntax/test_mutex.hpp"
#include "test_syntax/test_reference.hpp"
#include "test_utils/test_calc_tool.hpp"
#include "test_utils/test_db_connection_pool.hpp"
#include "test_utils/test_db_manager.hpp"
#include "test_utils/test_scheduler.hpp"
#include "test_utils/test_scheduler_lf.hpp"
#include "test_utils/test_rand.hpp"
#include "test_utils/test_reactor_epoll.hpp"
#include "test_utils/test_thread.hpp"
#include "test_utils/test_time.hpp"
#include "test_utils/test_timer.hpp"
#include "test_protocpp/test_msg.hpp"

void baseInit() {
    USING_NS_UTILS;
    TimeHelper::init();
    Logger::init();

    log_warn("~~~~~~~~~~~ TEST START ~~~~~~~~~~~~~~~~~~~");
}

int main() {
    baseInit();

    // proto 测试
    {
   	    // test msg
	    // TestMsg();
    }

    // logic work 测试
    {
        // test battle
        // TestBattle();
    }

    {
    	// test thread
    	// TestThread();

        // test time
        // TestTime();

    	// test timer
    	// TestTimer();

        // test rand
        // TestRand();

        // test Scheduler
        // TestScheduler();

        // test SchedulerLF
        // TestSchedulerLF();

        // test ReactorEpoll
        // TestReactorEpoll();

        // test calc tool
        // TestCalcTool();

        // test DBConnectionPool
        // TestDBConnectionPool();

        // test DBManager
        // TestDBManager();
    }

    // 语法测试
    {
        // test class
        // TestClass();

        // test reference
        // TestReference();

        // test cast
        // TestCast();

        // test mutex;
        TestMutex();
    }
    return 0;
}
