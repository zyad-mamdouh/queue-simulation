#include "QueueSystem.h"

QueueSystem::QueueSystem()
    : generator(0.8, 1, 5, 3),
      currentTime(0),
      totalCustomersServed(0),
      totalWaitingTime(0),
      totalQueueLength(0),
      totalBusyServerTime(0),
      stepsExecuted(0)
      {}

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

    // 1) Generate new customers. Some may be scheduled for future steps.
    std::vector<Customer*> generatedCustomers = generator.generate(currentTime);
    for (Customer* customer : generatedCustomers) {
        pendingArrivals.push_back(customer);
    }

    // 2) Add customers that arrive at this current time to the queue.
    for (auto it = pendingArrivals.begin(); it != pendingArrivals.end();) {
        Customer* customer = *it;
        if (customer != nullptr && customer->getArrivalTime() <= currentTime) {
            queue.enqueue(customer);
            it = pendingArrivals.erase(it);
        } else {
            ++it;
        }
    }

    // 3) Assign customers before servers process this step.
    assignCustomers();

    // 4) Busy servers process one unit of transaction time.
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
    serverList.updateServers();
    totalCustomersServed += customersCompletingThisStep;
    totalWaitingTime += waitingTimeFromCompletedCustomers;

    // 5) Assign again after servers that finished this step become free.
    assignCustomers();

    // 6) Every customer still waiting in queue waits one more time unit.
    queue.incrementWaitingTimes();

    totalQueueLength += queue.size();
    stepsExecuted++;
    currentTime++;
}

void QueueSystem::assignCustomers() {
    while (!queue.isEmpty()) {
        Server* availableServer = serverList.findAvailableServer();
        if (availableServer == nullptr) {
            break;
        }
        availableServer->serve(queue.dequeue());
    }
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
