#pragma once

#ifndef QUEUE_H
#define QUEUE_H

#include <queue>
#include <vector>
#include "Customer.h"
#include "../interfaces/QueueInterface.h"

class Queue : public QueueInterface {
private:
    std::queue<Customer*> customers;

public:
    void enqueue(Customer* customer) override;
    Customer* dequeue() override;
    void incrementWaitingTimes();
    std::vector<Customer*> getSnapshot() const;

    bool isEmpty() const;
    int size() const;
};

#endif