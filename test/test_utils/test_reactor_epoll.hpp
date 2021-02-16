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
#define NETWORK_SERVER_CLIENT false // true 服务端 false 客户端
static NetworkAccept s_networkAccept;

/*******************机器人*************/
static uint32 s_robotCount = 100;
static uint32 s_robotId = 0;
static uint32 s_maxMsg = 50;
class Robot {
public:
    Robot() { m_robotId = ++s_robotId; m_accuMsgCount = 0; m_close.store(false); }
    ~Robot() {}

    bool init(UTILS::ReactorEpollPtr pReactor, UTILS::SchedulerPtr pS, std::string ip, uint16 port) {
        std::shared_ptr<UTILS::StreamSocket> pSSocket = std::make_shared<UTILS::StreamSocket>(pReactor);
        if (pSSocket->getFd() == -1) {
            log_error("Create connect socket[%s]", pSSocket->getError().c_str());
            return false;
        }
        m_sendTimer = std::make_shared<UTILS::ReactorTimer>(pReactor);
        m_networkConnect = std::make_shared<NetworkSocket>();
        m_networkConnect->init(pReactor, pSSocket, pS, [this](int32 fd) { Robot::doClose(this->m_robotId); }, ip, port);
        m_networkConnect->connect(std::bind(&Robot::sendFunc, this));
        return true;
    }

    void sendFunc() {
        if (++m_accuMsgCount >= s_maxMsg) {
            m_networkConnect->onClose();
            return;
        }
        uint32 count = UTILS::Rand::randBetween((uint32)10, (uint32)100);
        uint32 secs = UTILS::Rand::randBetween((uint32)20, (uint32)1000);
        std::string str = UTILS::Rand::randString(count);
        m_networkConnect->send("机器人[" + std::to_string(m_robotId) +"] say : {" + str + "}");
        m_sendTimer->expiresFuncByMs(secs, [&](){ this->sendFunc(); });
    }

    static void doClose(uint32 robotId) {
        {
            std::lock_guard<std::mutex> lk(s_mapMutex);
            s_robotMap.erase(robotId);
        }
        checkNewRobot();
    }

    static void checkNewRobot() {
        log_info("Robot count[%u]", s_robotMap.size());
        if (s_robotMap.size() >= s_robotCount) {
            return;
        }

        uint32 count = s_robotCount - s_robotMap.size();
        {

            std::lock_guard<std::mutex> lk(s_mapMutex);
            for (uint32 i = 0; i < count; i++) {
                std::shared_ptr<Robot> pRobot = s_newFunc();
                if (pRobot) {
                    s_robotMap[pRobot->m_robotId] = pRobot;
                }
            }
        }
    }

    uint32 m_robotId;
    uint32 m_accuMsgCount;
    std::atomic<bool> m_close;
    NSocketPtr m_networkConnect;
    std::shared_ptr<UTILS::ReactorTimer> m_sendTimer;
public:
    static std::mutex s_mapMutex;
    static std::map<uint32, std::shared_ptr<Robot>> s_robotMap;
    static std::function<std::shared_ptr<Robot>()> s_newFunc;
};
std::mutex Robot::s_mapMutex;
std::map<uint32, std::shared_ptr<Robot>> Robot::s_robotMap;
std::function<std::shared_ptr<Robot>()> Robot::s_newFunc;
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

#if NETWORK_TASK_TEST
    {
#if NETWORK_SERVER_CLIENT
        {
           std::shared_ptr<StreamSocket> pSSocket = std::make_shared<StreamSocket>(pRE);
           if (pSSocket->getFd() == -1) {
               log_error("Create server listen socket[%s]", pSSocket->getError().c_str());
               pS->stop();
               return;
           }
           s_networkAccept.init(pSSocket, pRE, pS);
           if (!s_networkAccept.listen("0.0.0.0", 7777)) {
                pS->stop();
                return;
           }
           s_networkAccept.accept();
        }
#elif NETWORK_SERVER_CLIENT == 0
        {
            Robot::s_newFunc = [&pRE, &pS] () -> std::shared_ptr<Robot> {
                std::shared_ptr<Robot> pRobot = std::make_shared<Robot>();
                if (!pRobot->init(pRE, pS, "0.0.0.0", 7777)) {
                    return NULL;
                }
                return pRobot;
            };
            Robot::checkNewRobot();
        }
#endif
    }
#endif
}

#endif
