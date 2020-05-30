#include "server_base.hpp"

#include <stddef.h>

NS_SERVICE_CORE_BEGIN

ServerBase* ServerBase::m_server = NULL;

ServerBase::ServerBase() {
    m_server = this;
}

ServerBase::~ServerBase() {
    m_server = NULL;
}

ServerBase* ServerBase::getInstance() {
    return m_server;
}

NS_SERVICE_CORE_END
