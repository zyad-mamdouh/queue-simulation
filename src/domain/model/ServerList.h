#pragma once


#include <vector>
#include "Server.h"

class ServerList {
private:
    std::vector<Server*> servers;

public:
    void addServer(Server* server);

    Server* findAvailableServer();
    void updateServers();
    const std::vector<Server*>& getServers() const;
};

