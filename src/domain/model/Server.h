#pragma once


#include "Customer.h"
#include "ServerStatus.hpp"

class Server  {
private:
    Customer* currentCustomer;
    ServerStatus status;

public:
    Server();

    // Core behavior
    void serve(Customer* customer);
    bool isAvailable() const;

    void processTimeStep();  

    Customer* getCurrentCustomer() const;
};


