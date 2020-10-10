#ifndef TEST_UTILS_TEST_REACTOR_EPOLL_HPP
#define TEST_UTILS_TEST_REACTOR_EPOLL_HPP

#include "utils/log.hpp"
#include "utils/log_helper.hpp"
#include "utils/macros.hpp"
#include "utils/reactor_epoll.hpp"
#include "utils/scope_guard.hpp"
#include "utils/scheduler_leader_follower.hpp"
#include "utils/scheduler_unit.hpp"
#include "utils/scheduler_safe_unit.hpp"
#include "utils/thread_pool.hpp"

// 批量任务测试
#define BATCH_TASK_TEST false
#define BATCH_TASK_COUNT 1000

// 网络测试
#define NETWORK_TASK_TEST true
#ifdef NETWORK_TASK_TEST
#   include "test_stream_socket.hpp"
static NetworkAccept s_networkAccept;
static NetworkSocket s_networkConnect;
#endif

void TestReactorEpoll() {
    USING_NS_UTILS;
    FUNC_TRACE;
    std::shared_ptr<ReactorEpoll> pRE = std::make_shared<ReactorEpoll>();
    std::shared_ptr<SchedulerLF> pS = std::make_shared<SchedulerLF>(SCHEDULE_LF_MODE_REACTOR|SCHEDULE_LF_MODE_TASK_LIST);
    if (!pS->init(pRE)) {
        log_error("SchedulerLF init failed.");
        return;
    }
    if (!pRE->init(pS)) {
        log_error("Reactor Epoll init failed.");
        return;
    }

    ThreadPool pool;
    pool.createThread([&pS](){ pS->run(); }, 5);
    pS->start();

    if (BATCH_TASK_TEST) {
        uint32 times = BATCH_TASK_COUNT;
        pool.createThread([&times, &pS]() {
                UnitPtr pUnit = std::make_shared<SchdeulerUnit>([&times, &pS]() {
                            times--;
                            log_debug("Thread[%u] do task times[%u]", std::this_thread::get_id(), times);
                            for (int i = 0 ; i< 200000; i++) {
                                int j = 1;
                                j += 1;
                            }
                            if (0 == times) {
                                pS->stop();
                            }
                        });

                uint32 count = times;
                for (uint32 i = 0; i < count; i++) {
                    pS->post(pUnit);
                }
            }, 1);
    }

#ifdef NETWORK_TASK_TEST
    {
        // 开启服务
        {
           std::shared_ptr<StreamSocket> pSSocket = std::make_shared<StreamSocket>(pRE);
           if (pSSocket->getFd() == -1) {
               log_error("Create server listen socket[%s]", pSSocket->getError().c_str());
               pS->stop();
               return;
           }
           s_networkAccept.init(pSSocket, pRE);
           if (!s_networkAccept.listen("0.0.0.0", 7777)) {
                pS->stop();
                return;
           }
           s_networkAccept.accept();
        }

        // 自产自销测试
        // 发起链接
        do {
            std::shared_ptr<StreamSocket> pSSocket = std::make_shared<StreamSocket>(pRE);
            if (pSSocket->getFd() == -1) {
                log_error("Create connect socket[%s]", pSSocket->getError().c_str());
                break;
            }
            s_networkConnect.init(pSSocket, "0.0.0.0", 7777);
            s_networkConnect.connect();
        } while(0);
    }
#endif
}

#endif
