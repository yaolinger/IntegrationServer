#include "reactor_timer.hpp"

#include <mutex>
#include "scheduler_unit.hpp"
#include "time_helper.hpp"
#include "log.hpp"

NS_UTILS_BEGIN

std::once_flag ReactorTimer::s_once;
std::mutex ReactorTimer::s_secUnitMutex;
std::map<uint32, std::list<UnitPtr>> ReactorTimer::s_secUnitMap;
std::mutex ReactorTimer::s_msUnitMutex;
std::map<uint64, std::list<UnitPtr>> ReactorTimer::s_msUnitMap;

ReactorTimer::ReactorTimer(ReactorPtr pReactor) : m_reactor(pReactor) {
    if (m_reactor) {
        std::call_once(s_once, [this](){ this->m_reactor->setTimerCheck(getTimerTask); });
    }
    m_cancelSign.store(false);
}

ReactorTimer::~ReactorTimer() {
}

void ReactorTimer::expiresFunc(uint32 seconds, std::function<void()> func) {
    int32 sysSeconds = TimeHelper::getSystemCurrentSecond();
    uint32 nextSeconds = sysSeconds + seconds;
    {
        auto timerFunc = [this, func]() {
            // 定时器取消
            if (this->m_cancelSign.load()) {
                return;
            }
            func();
        };
        SchdeulerUnitPtr pUnit = std::make_shared<SchdeulerUnit>(timerFunc);
        std::lock_guard<std::mutex> lk(s_secUnitMutex);
        auto iter = s_secUnitMap.find(nextSeconds);
        if (iter == s_secUnitMap.end()) {
            std::list<UnitPtr> unitList;
            unitList.push_back(pUnit);
            s_secUnitMap[nextSeconds] = unitList;
        } else {
            iter->second.push_back(pUnit);
        }
    }
}

void ReactorTimer::expiresFuncByMs(uint64 milliseconds, std::function<void()> func) {
    int64 sysMs = TimeHelper::getSystemCurrentMillisecond();
    uint64 nextMs = sysMs + milliseconds;
    {
        auto timerFunc = [this, func]() {
            // 定时器取消
            if (this->m_cancelSign.load()) {
                return;
            }
            func();
        };
        SchdeulerUnitPtr pUnit = std::make_shared<SchdeulerUnit>(timerFunc);
        std::lock_guard<std::mutex> lk(s_msUnitMutex);
        auto iter = s_msUnitMap.find(nextMs);
        if (iter == s_msUnitMap.end()) {
            std::list<UnitPtr> unitList;
            unitList.push_back(pUnit);
            s_msUnitMap[nextMs] = unitList;
        } else {
            iter->second.push_back(pUnit);
        }

    }
}

void ReactorTimer::cancelFunc() {
    m_cancelSign.store(true);
}

void ReactorTimer::getTimerTask(std::list<UnitPtr>& taskList) {
    uint32 sysNow = TimeHelper::getSystemCurrentSecond();
    {
        std::lock_guard<std::mutex> lk(s_secUnitMutex);
		for (auto iter = s_secUnitMap.begin(); iter != s_secUnitMap.end();) {
			if (iter->first > sysNow) {
				break;
			}
     		// 追加任务
            taskList.splice(taskList.end(), iter->second);
			iter = s_secUnitMap.erase(iter);
		}
    }
    uint64 sysNowMs = TimeHelper::getSystemCurrentMillisecond();
    {
        std::lock_guard<std::mutex> lk(s_msUnitMutex);
		for (auto iter = s_msUnitMap.begin(); iter != s_msUnitMap.end();) {
			if (iter->first > sysNowMs) {
				break;
			}
		    // 追加任务
            taskList.splice(taskList.end(), iter->second);
        	iter = s_msUnitMap.erase(iter); 
		}
    }
}

NS_UTILS_END;
