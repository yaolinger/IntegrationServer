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
#include "test_stream_socket.hpp"

// 批量任务测试
#define BATCH_TASK_TEST false
#define BATCH_TASK_COUNT 1000

// 网络测试
#define NETWORK_TASK_TEST true
#ifdef NETWORK_TASK_TEST
/*******************服务端*************/
//#define NETWORK_SERVER true
//static NetworkAccept s_networkAccept;

/*******************机器人*************/
#define NETWORK_CLIENT true
static uint32 s_robotCount = 50;
static uint32 s_robotId = 0;
class Robot {
public:
    Robot() { m_robotId = ++s_robotId; m_accuMsgCount = 0; m_close.store(false); }
    ~Robot() {}

    bool init(UTILS::ReactorEpollPtr pReactor, std::string ip, uint16 port) {
        std::shared_ptr<UTILS::StreamSocket> pSSocket = std::make_shared<UTILS::StreamSocket>(pReactor);
        if (pSSocket->getFd() == -1) {
            log_error("Create connect socket[%s]", pSSocket->getError().c_str());
            return false;
        }
        m_sendTimer = std::make_shared<UTILS::ReactorTimer>(pReactor);
        m_networkConnect.init(pReactor, pSSocket, ip, port);
        m_networkConnect.connect();
        sendFunc();
        return true;
    }

    void sendFunc() {
        m_accuMsgCount++;
        uint32 count = UTILS::Rand::randBetween((uint32)10, (uint32)1000);
        uint32 secs = UTILS::Rand::randBetween((uint32)30, (uint32)80);
        std::string str = UTILS::Rand::randString(count);
        m_networkConnect.send("机器人[" + std::to_string(m_robotId) +"] say : {" + str + "}");
        m_sendTimer->expiresFuncByMs(secs, [&](){ this->sendFunc(); });
    }

    uint32 m_robotId;
    uint32 m_accuMsgCount;
    std::atomic<bool> m_close;
    NetworkSocket m_networkConnect;
    std::shared_ptr<UTILS::ReactorTimer> m_sendTimer;
};
std::map<uint32, std::shared_ptr<Robot>> s_robotMap;
std::shared_ptr<UTILS::ReactorTimer> s_checkTimer;
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
#ifdef NETWORK_SERVER
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
#elif NETWORK_CLIENT
        {
            // 初始化机器人
            for (uint32 i = 0; i < s_robotCount; i++) {
                std::shared_ptr<Robot> pRobot = std::make_shared<Robot>();
                if (!pRobot->init(pRE, "0.0.0.0", 7777)) {
                    log_error("Robot[%u] init faild.", pRobot->m_robotId);
                } else {
                    s_robotMap[pRobot->m_robotId] = pRobot;
                }
            }
        }
#endif
    }
#endif
}

#endif
