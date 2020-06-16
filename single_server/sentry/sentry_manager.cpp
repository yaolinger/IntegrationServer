#include "sentry_manager.hpp"

#include "utils/log.hpp"
#include "protocpp/op_cmd.hpp"
#include "protocpp/protos/server.pb.h"

USING_NS_UTILS;

uint64 ObjectManager::s_globalUid = 0;
UTILS::TimerPtr ObjectManager::s_timer;

std::map<uint64, ObjectPtr> ObjectManager::s_objectMap;

void Object::checkActive(int32 now) {
	if (m_activeTime + LINK_ACTIVE_TIME < now) {
	    proto::server::MsgPing request;
	    global::pServer->sendMsgBySocket(m_socketPtr, PROTOCPP::MsgPing, request);
    }
}

void ObjectManager::addConnector(const BOOST_NETWORK::TcpSocketPtr& s) {
    if (NULL == s) {
        log_error("NULL == s");
        return;
    }
    ObjectPtr ob = std::make_shared<Object>(s, getIdleUid());
    if (addObject(ob)) {
        log_info("Add object[%lu] at socket[%d][%s:%u]", ob->getUid(), s->getSocketId(), s->getIp().c_str(), s->getPort());
    } else {
        log_error("Add object[%lu] failed at socket[%d][%s:%u]", ob->getUid(), s->getSocketId(), s->getIp().c_str(), s->getPort());
    }
}

void ObjectManager::delConnector(const BOOST_NETWORK::TcpSocketPtr& s) {
    if (NULL == s) {
        log_error("NULL == s");
        return;
    }
    if (delObject(s->getBindUid())) {
        log_info("Del object[%lu] at socket[%d][%s:%u]", s->getBindUid(), s->getSocketId(), s->getIp().c_str(), s->getPort());
    } else {
        log_error("Del object[%lu] failed at socket[%d][%s:%u]", s->getBindUid(), s->getSocketId(), s->getIp().c_str(), s->getPort());
    }
}

bool ObjectManager::init() {
	s_timer.reset(new UTILS::TimerBase(1));
	s_timer->afterFunc(5, ObjectManager::onTimer);
	return true;
}

void ObjectManager::close() {
	s_timer->cancel();
}

bool ObjectManager::addObject(const ObjectPtr& ob) {
    if (NULL != findObject(ob->getUid())) {
        return false;
    }

    s_objectMap.insert(std::make_pair(ob->getUid(), ob));
    return true;
}

bool ObjectManager::delObject(uint64 uid) {
    auto ob = findObject(uid);
    if (NULL == ob) {
        return false;
    }

    s_objectMap.erase(uid);
    return true;
}

ObjectPtr ObjectManager::findObject(uint64 uid) {
    auto iter = s_objectMap.find(uid);
    if (iter == s_objectMap.end() || NULL == iter->second) {
        return NULL;
    }
    return iter->second;
}

uint64 ObjectManager::getIdleUid() {
    return s_globalUid++;
}

void ObjectManager::onTimer() {
	//log_info("ObjectManager timer callback~");
	int32 now = TimeHelper::getCurrentSecond();

	for (auto iter = s_objectMap.begin(); iter != s_objectMap.end();) {
		if (NULL == iter->second) {
			iter++;
			continue;
		}
		ObjectPtr ob = iter->second;
		if (ob->isTimeout(now)) {
		    global::pServer->postCloseSocket(ob->getSocketPtr());
			iter = s_objectMap.erase(iter);
		    continue;
        }
        ob->checkActive(now);
        iter++;
	}
	s_timer->afterFunc(5, ObjectManager::onTimer);
}
