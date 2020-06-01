#ifndef SINGLE_SERVER_SINGLE_SERVER_HPP
#define SINGLE_SERVER_SINGLE_SERVER_HPP

#include "service_core/boost_server.hpp"

class SingleServer : public service_core::BoostServer {
public:
    SingleServer() {}
    ~SingleServer() {}

public:
    const char* getServerName() { return "SingleServer"; }

    bool onInitServer();

    void onRunServer();

    bool onCloseServer();
};
#endif
