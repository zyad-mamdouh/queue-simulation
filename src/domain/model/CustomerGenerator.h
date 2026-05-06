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

    // Random number generation
    std::mt19937 randomEngine;
    std::poisson_distribution<int> arrivalCountDistribution; // For number of arrivals per time unit
    std::uniform_int_distribution<int> serviceTimeDistribution; // For service time of each customer
    std::geometric_distribution<int> arrivalDelayDistribution;  // For delay between arrivals

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
