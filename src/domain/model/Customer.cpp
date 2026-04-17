#include "Customer.h"

// Constructor
Customer::Customer(int customerNumber, int arrivalTime, int transactionTime)
    : customerNumber(customerNumber),
      arrivalTime(arrivalTime),
      waitingTime(0),
      transactionTime(transactionTime) {}

// Increase waiting time
void Customer::incrementWaitingTime() {
    waitingTime++;
}

// Reduce transaction time (during service)
void Customer::reduceTransactionTime() {
    if (transactionTime > 0) {
        transactionTime--;
    }
}

// Getters
int Customer::getWaitingTime() const {
    return waitingTime;
}

int Customer::getTransactionTime() const {
    return transactionTime;
}

int Customer::getCustomerNumber() const {
    return customerNumber;
}

int Customer::getArrivalTime() const {
    return arrivalTime;
}