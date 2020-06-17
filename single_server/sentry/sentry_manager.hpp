#ifndef SINGLE_SERVER_SENTRY_MANAGER_HPP
#define SINGLE_SERVER_SENTRY_MANAGER_HPP

#include <map>
#include <memory>

#include "boost_network/tcp_socket.hpp"
#include "main/server_global.hpp"
#include "main/single_server.hpp"
#include "utils/log.hpp"
#include "utils/time_manager.hpp"
#include "utils/time_helper.hpp"

// socket 绑定对象
class Object {
public:
	Object(const BOOST_NETWORK::TcpSocketPtr& s, uint64 uid) : m_socketPtr(s), m_uid(uid) {
		m_socketPtr->bindUid(uid);
		m_activeTime = UTILS::TimeHelper::getCurrentSecond();
	};

public:
	BOOST_NETWORK::TcpSocketPtr getSocketPtr() { return m_socketPtr; }

	uint64 getUid() { return m_uid; }

	void checkActive(int32 now);

	bool isTimeout(int32 now) { return m_activeTime + LINK_TIMEOUT < now; }

	void setActive() { m_activeTime = UTILS::TimeHelper::getCurrentSecond(); }

public:
    // tcp 极限测试
    // 连续发包 立即关闭链接测试
    // 目的:保证关闭链接前的数据全部发送
    void testLimitTcp(uint32 times);

private:
	BOOST_NETWORK::TcpSocketPtr m_socketPtr;      // socket
	uint64 m_uid;                                 // 唯一id
	int32 m_activeTime;                          // 活跃时间
};

typedef std::shared_ptr<Object> ObjectPtr;

// 对象管理器
class ObjectManager {
public:
    static void addConnector(const BOOST_NETWORK::TcpSocketPtr& s);

    static void delConnector(const BOOST_NETWORK::TcpSocketPtr& s);

public:
	static bool init();

	static void close();

   	static bool addObject(const ObjectPtr& ob);

    static bool delObject(uint64 uid);

	static ObjectPtr findObject(uint64 uid);

    static uint64 getIdleUid();

public:
	// 计时器回调
	static void onTimer();

private:
    static std::map<uint64, ObjectPtr> s_objectMap;

	static uint64 s_globalUid;

	static UTILS::TimerPtr s_timer;
};


#endif
