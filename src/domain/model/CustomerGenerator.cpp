#include "CustomerGenerator.h"
#include <algorithm>
#include <random>
#include <stdexcept>

CustomerGenerator::CustomerGenerator(
    double arrivalRate,
    int minServiceTime,
    int maxServiceTime,
    int maxArrivalDelay
)
    : arrivalRate(arrivalRate),
      minServiceTime(minServiceTime),
      maxServiceTime(maxServiceTime),
      maxArrivalDelay(maxArrivalDelay),
      nextCustomerNumber(1),
      randomEngine(std::random_device{}()),
      arrivalCountDistribution(arrivalRate),
      serviceTimeDistribution(minServiceTime, maxServiceTime),
      arrivalDelayDistribution(0.6) {
    if (arrivalRate < 0.0) {
        throw std::invalid_argument("arrivalRate must be non-negative");
    }
    if (minServiceTime <= 0 || maxServiceTime < minServiceTime) {
        throw std::invalid_argument("invalid service time range");
    }
    if (maxArrivalDelay < 0) {
        throw std::invalid_argument("maxArrivalDelay must be non-negative");
    }
}

std::vector<Customer*> CustomerGenerator::generate(int currentTime) {
    std::vector<Customer*> customers;
    const int count = arrivalCountDistribution(randomEngine);

    for (int i = 0; i < count; i++) {
        const int arrivalTime = currentTime + generateArrivalDelay();
        const int serviceTime = generateServiceTime();
        customers.push_back(new Customer(nextCustomerNumber++, arrivalTime, serviceTime));
    }

    return customers;
}

int CustomerGenerator::generateServiceTime() {
    return serviceTimeDistribution(randomEngine);
}

int CustomerGenerator::generateArrivalDelay() {
    return std::min(arrivalDelayDistribution(randomEngine), maxArrivalDelay);
}
