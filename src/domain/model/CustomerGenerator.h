#ifndef CUSTOMER_GENERATOR_H
#define CUSTOMER_GENERATOR_H

#include <random>
#include <vector>
#include "Customer.h"

class CustomerGenerator {
private:
    double arrivalRate;
    int minServiceTime;
    int maxServiceTime;
    int maxArrivalDelay;
    int nextCustomerNumber;

    std::mt19937 randomEngine;
    std::poisson_distribution<int> arrivalCountDistribution;
    std::uniform_int_distribution<int> serviceTimeDistribution;
    std::geometric_distribution<int> arrivalDelayDistribution;

    int generateServiceTime();
    int generateArrivalDelay();

public:
    CustomerGenerator(
        double arrivalRate,
        int minServiceTime,
        int maxServiceTime,
        int maxArrivalDelay
    );

    std::vector<Customer*> generate(int currentTime);
};

#endif
