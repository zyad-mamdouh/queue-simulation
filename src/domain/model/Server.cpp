#include "Server.h"

// Constructor
Server::Server() : currentCustomer(nullptr), status(ServerStatus::FREE) {}

// Assign customer to server
void Server::serve(Customer* customer) {
    if (customer == nullptr) return;

    currentCustomer = customer;
    status = ServerStatus::BUSY;
}

// Check availability
bool Server::isAvailable() const {
    return status == ServerStatus::FREE;
}

// Process one time step
void Server::processTimeStep() {
    if (currentCustomer == nullptr) return;

    // Reduce customer's transaction time
    currentCustomer->reduceTransactionTime();

    // If finished
    if (currentCustomer->getTransactionTime() == 0) {
        currentCustomer = nullptr;
        status = ServerStatus::FREE;
    }
}

// Getter
Customer* Server::getCurrentCustomer() const {
    return currentCustomer;
}