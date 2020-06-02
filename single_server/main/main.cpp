#include "server_global.hpp"
#include "single_server.hpp"

SingleServer* global::pServer = NULL;

int main() {
	SingleServer server;
    global::pServer = &server;
    global::pServer->runServer();
}
