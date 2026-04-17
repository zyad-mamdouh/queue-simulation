#include "QueueSystem.h"

QueueSystem::QueueSystem()
    : currentTime(0),
      totalCustomersServed(0),
      totalWaitingTime(0),
      totalQueueLength(0),
      totalBusyServerTime(0),
      stepsExecuted(0) {}

void QueueSystem::addCustomer(Customer* customer) {
    if (customer != nullptr) {
        pendingArrivals.push_back(customer);
    }
}

void QueueSystem::addServer(Server* server) {
    serverList.addServer(server);
}

void QueueSystem::runSimulationStep() {
    int customersCompletingThisStep = 0;
    long long waitingTimeFromCompletedCustomers = 0;

    // Collect metrics from server state at the beginning of the step.
    const std::vector<Server*>& servers = serverList.getServers();
    for (Server* server : servers) {
        Customer* active = server->getCurrentCustomer();
        if (active != nullptr) {
            totalBusyServerTime++;
            if (active->getTransactionTime() <= 1) {
                customersCompletingThisStep++;
                waitingTimeFromCompletedCustomers += active->getWaitingTime();
            }
        }
    }

    // 1) Busy servers process one unit of transaction time.
    serverList.updateServers();
    totalCustomersServed += customersCompletingThisStep;
    totalWaitingTime += waitingTimeFromCompletedCustomers;

    // 2) Every customer still waiting in queue waits one more time unit.
    queue.incrementWaitingTimes();

    // 3) Add customers that arrive at this current time.
    for (auto it = pendingArrivals.begin(); it != pendingArrivals.end();) {
        Customer* customer = *it;
        if (customer != nullptr && customer->getArrivalTime() <= currentTime) {
            queue.enqueue(customer);
            it = pendingArrivals.erase(it);
        } else {
            ++it;
        }
    }

    // 4) Send the customer at the front of the FIFO queue to free servers.
    while (!queue.isEmpty()) {
        Server* availableServer = serverList.findAvailableServer();
        if (availableServer == nullptr) {
            break;
        }
        availableServer->serve(queue.dequeue());
    }

    totalQueueLength += queue.size();
    stepsExecuted++;
    currentTime++;
}

void QueueSystem::runSimulation(int steps) {
    if (steps <= 0) {
        return;
    }
    for (int i = 0; i < steps; i++) {
        runSimulationStep();
    }
}

const Queue& QueueSystem::getQueue() const {
    return queue;
}

const ServerList& QueueSystem::getServerList() const {
    return serverList;
}

int QueueSystem::getCurrentTime() const {
    return currentTime;
}

int QueueSystem::getTotalCustomersServed() const {
    return totalCustomersServed;
}

long long QueueSystem::getTotalWaitingTime() const {
    return totalWaitingTime;
}

double QueueSystem::getAverageWaitingTime() const {
    if (totalCustomersServed == 0) {
        return 0.0;
    }
    return static_cast<double>(totalWaitingTime) / totalCustomersServed;
}

double QueueSystem::getAverageQueueLength() const {
    if (stepsExecuted == 0) {
        return 0.0;
    }
    return static_cast<double>(totalQueueLength) / stepsExecuted;
}

double QueueSystem::getServerUtilization() const {
    const int serverCount = static_cast<int>(serverList.getServers().size());
    if (serverCount == 0 || stepsExecuted == 0) {
        return 0.0;
    }
    const long long totalServerCapacity = static_cast<long long>(serverCount) * stepsExecuted;
    return static_cast<double>(totalBusyServerTime) / totalServerCapacity;
}