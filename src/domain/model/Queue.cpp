#include "Queue.h"

void Queue::enqueue(Customer* customer) {
    if (customer != nullptr) {
        customers.push(customer);
    }
}
//return object 
Customer* Queue::dequeue() {
    if (customers.empty()) return nullptr;

    Customer* front = customers.front();
    customers.pop();
    return front;
}

void Queue::incrementWaitingTimes() {
    const int customerCount = static_cast<int>(customers.size());
    for (int i = 0; i < customerCount; i++) {
        Customer* front = customers.front();
        customers.pop();
        front->incrementWaitingTime();
        customers.push(front);
    }
}
// Copy the queue because to vector without modifying the original queue
std::vector<Customer*> Queue::getSnapshot() const {
    std::vector<Customer*> snapshot;
    std::queue<Customer*> copy = customers;
    while (!copy.empty()) {
        snapshot.push_back(copy.front());
        copy.pop();
    }
    return snapshot;
}

bool Queue::isEmpty() const {
    return customers.empty();
}

int Queue::size() const {
    return customers.size();
}