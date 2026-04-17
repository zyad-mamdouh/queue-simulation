#include "SimulationController.h"

SimulationController::SimulationController() = default;

void SimulationController::addCustomer(Customer* customer) {
    system.addCustomer(customer);
}

void SimulationController::addServer(Server* server) {
    system.addServer(server);
}

void SimulationController::runStep() {
    system.runSimulationStep();
}

void SimulationController::runSimulation(int steps) {
    system.runSimulation(steps);
}

const Queue& SimulationController::getQueue() const {
    return system.getQueue();
}

const ServerList& SimulationController::getServers() const {
    return system.getServerList();
}

int SimulationController::getCurrentTime() const {
    return system.getCurrentTime();
}

int SimulationController::getTotalCustomersServed() const {
    return system.getTotalCustomersServed();
}

long long SimulationController::getTotalWaitingTime() const {
    return system.getTotalWaitingTime();
}

double SimulationController::getAverageWaitingTime() const {
    return system.getAverageWaitingTime();
}

double SimulationController::getAverageQueueLength() const {
    return system.getAverageQueueLength();
}

double SimulationController::getServerUtilization() const {
    return system.getServerUtilization();
}