#pragma once


class Customer {
private:
    int customerNumber;
    int arrivalTime;
    int waitingTime;
    int transactionTime;

public:
    
    Customer(int customerNumber, int arrivalTime, int transactionTime);

   
    void incrementWaitingTime();
    void reduceTransactionTime();

  
    int getWaitingTime() const;
    int getTransactionTime() const;
    int getCustomerNumber() const;
    int getArrivalTime() const;
};

