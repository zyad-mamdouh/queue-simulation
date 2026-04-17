#ifndef QUEUE_INTERFACE_H
#define QUEUE_INTERFACE_H

#include "Customer.h"

class QueueInterface {
public:
    virtual ~QueueInterface() = default;

    virtual void enqueue(Customer* customer) = 0;
    virtual Customer* dequeue() = 0;
};

#endif