#pragma once

#ifndef SIMULATION_CONTROLLER_H
#define SIMULATION_CONTROLLER_H

#include "../domain/system/QueueSystem.h"

class SimulationController {
private:
    QueueSystem system;

public:
    SimulationController();

    void addCustomer(Customer* customer);
    void addServer(Server* server);

    void runStep();
    void runSimulation(int steps);

    // For UI
    const Queue& getQueue() const;
    const ServerList& getServers() const;
    int getCurrentTime() const;
    int getTotalCustomersServed() const;
    long long getTotalWaitingTime() const;
    double getAverageWaitingTime() const;
    double getAverageQueueLength() const;
    double getServerUtilization() const;
};

#endif