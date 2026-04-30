#ifndef QUEUE_SYSTEM_H
#define QUEUE_SYSTEM_H

#include <vector>
#include "../model/CustomerGenerator.h"
#include "../model/Queue.h"
#include "../model/ServerList.h"

class QueueSystem {
private:
    Queue queue;
    ServerList serverList;
    CustomerGenerator generator;
    std::vector<Customer*> pendingArrivals;
    int currentTime;
    int totalCustomersServed;
    long long totalWaitingTime;
    long long totalQueueLength;
    long long totalBusyServerTime;
    int stepsExecuted;

public:
    QueueSystem();

    void addCustomer(Customer* customer);
    void addServer(Server* server);

    void runSimulationStep();
    void runSimulation(int steps);

    // Getters (for UI)
    const Queue& getQueue() const;
    const ServerList& getServerList() const;
    int getCurrentTime() const;
    int getTotalCustomersServed() const;
    long long getTotalWaitingTime() const;
    double getAverageWaitingTime() const;
    double getAverageQueueLength() const;
    double getServerUtilization() const;

private:
    void assignCustomers();
};

#endif
