#include "ServerList.h"

void ServerList::addServer(Server* server) {
    if (server != nullptr) {
        servers.push_back(server);
    }
}

// Find first free server O(n)
Server* ServerList::findAvailableServer() {
    for (auto server : servers) {
        if (server->isAvailable()) {
            return server;
        }
    }
    return nullptr;
}

// Update all servers (time step)
// updateServers -> processTimeStep -> reduceTransactionTime 
void ServerList::updateServers() {
    for (auto server : servers) {
        server->processTimeStep();
    }
}

const std::vector<Server*>& ServerList::getServers() const {
    return servers;
}
