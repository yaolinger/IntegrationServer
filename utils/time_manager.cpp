#include "time_manager.hpp"

#include <boost/asio/placeholders.hpp>
#include <boost/bind.hpp>

#include "log.hpp"
#include "time_base.hpp"
#include "time_helper.hpp"

NS_UTILS_BEGIN

std::multimap<uint32, TimerPtr> TimerManager::m_timerMap;

std::shared_ptr<boost::asio::deadline_timer> TimerManager::m_timer = NULL;

void TimerManager::init(boost::asio::io_service& ios) {
    m_timer.reset(new boost::asio::deadline_timer(ios));
    m_timer->expires_from_now(TIMER_MANAGER_DURING);
    m_timer->async_wait(boost::bind(&TimerManager::onTimer, boost::asio::placeholders::error));
}

void TimerManager::cancel() {
    if (NULL != m_timer) {
        m_timerMap.clear();
        m_timer->cancel();
        m_timer.reset();
    }
}

bool TimerManager::registrate(TimerPtr& pTimer) {
    if (NULL == pTimer) {
        log_error("NULL == pTimer.");
        return false;
    }
    return true;
}

void TimerManager::onTimer(const boost::system::error_code& ec) {
   	if (ec == boost::asio::error::operation_aborted) {
		return;
	} else if (ec){
		log_error("Timer error: %s", ec.message().c_str());
		return;
	}

    // 获取当前时间
    int32 now = TimeHelper::getCurrentSecond();

    // 待执行timer vector
    std::vector<TimerPtr> waitingRunVec;

    // 获取可执行timer
    for (auto iter = m_timerMap.begin(); iter != m_timerMap.end();) {
        if (NULL == iter->second) {
            iter++;
            break;
        }

        // map key排序(由小到大)
        if (iter->first > (uint32)now) {
            break;
        }

        // map容器遍历删除
        // c98 map.erase(iter++)
        // c11 map.erase(iter++)  | iter = map.erase(iter)
        // c11 map.erase 增加了返回值
        waitingRunVec.push_back(iter->second);
        iter = m_timerMap.erase(iter);
    }

    // 遍历执行
    for (TimerPtr iter : waitingRunVec) {
        if (NULL == iter) {
            continue;
        }

        // 执行
        iter->doComplete();

        // 可再执行重新加入容器
        if (!iter->isOver()) {
            m_timerMap.insert(std::make_pair(iter->getNextTime(), iter));
        }
    }

    if (NULL != m_timer) {
        m_timer->expires_from_now(TIMER_MANAGER_DURING);
        m_timer->async_wait(boost::bind(&TimerManager::onTimer, boost::asio::placeholders::error));
    }
}

NS_UTILS_END
