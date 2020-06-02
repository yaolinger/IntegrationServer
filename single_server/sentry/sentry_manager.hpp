#ifndef SINGLE_SERVER_SENTRY_MANAGER_HPP
#define SINGLE_SERVER_SENTRY_MANAGER_HPP

#include <map>
#include <memory>

#include "boost_network/tcp_socket.hpp"
#include "utils/log.hpp"

// socket 绑定对象
class Object {
public:
	Object(const BOOST_NETWORK::TcpSocketPtr& s, uint64 uid) : m_socketPtr(s), m_uid(uid) {
		m_socketPtr->bindUid(uid);
	};

public:
	BOOST_NETWORK::TcpSocketPtr getSocketPtr() { return m_socketPtr; }

	uint64 getUid() { return m_uid; }

private:
	BOOST_NETWORK::TcpSocketPtr m_socketPtr;      // socket
	uint64 m_uid;                                 // 唯一id
};

typedef std::shared_ptr<Object> ObjectPtr;


// 对象管理器
class ObjectManager {
public:
    static void addConnector(const BOOST_NETWORK::TcpSocketPtr& s);

    static void delConnector(const BOOST_NETWORK::TcpSocketPtr& s);

public:
   	static bool addObject(const ObjectPtr& ob);

    static bool delObject(uint64 uid);

	static ObjectPtr findObject(uint64 uid);

    static uint64 getIdleUid();

private:
    static std::map<uint64, ObjectPtr> s_objectMap;

	static uint64 s_globalUid;
};


#endif
