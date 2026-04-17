#include "ConsoleUI.h"
#include <iostream>
#include <vector>
#include <iomanip>

using namespace std;

void ConsoleUI::run() {
    int choice;

   
    controller.addServer(new Server());
    controller.addServer(new Server());

    controller.addCustomer(new Customer(1, 0, 1));
    controller.addCustomer(new Customer(2, 0, 5));
    controller.addCustomer(new Customer(3, 0, 2));
    
    controller.addCustomer(new Customer(4, 0, 7));
    controller.addCustomer(new Customer(5, 4, 2));
    controller.addCustomer(new Customer(6, 0, 2));
    
    controller.addCustomer(new Customer(7, 0, 3));
    controller.addCustomer(new Customer(8, 2, 5));
    controller.addCustomer(new Customer(9, 0, 11));
    
    controller.addCustomer(new Customer(10, 0, 3));
    controller.addCustomer(new Customer(11, 0, 5));
    controller.addCustomer(new Customer(12, 9, 1));

    while (true) {
        displayMenu();
        cin >> choice;

        if (choice == 1) {
            controller.runStep();
            displayState();
        } else if (choice == 2) {
            int steps;
            cout << "Enter number of steps: ";
            cin >> steps;
            for (int i = 0; i < steps; i++) {
                controller.runStep();
                displayState();
            }
            displayMetrics();
        } else if (choice == 0) {
            break;
        }
    }
}

void ConsoleUI::displayMenu() {
    cout << "\n1. Run Simulation Step\n";
    cout << "2. Run Simulation N Steps\n";
    cout << "0. Exit\n";
    cout << "Choice: ";
}

void ConsoleUI::displayState() {
    cout << "\n--- Simulation Step Executed ---\n";
    cout << "Current time: " << controller.getCurrentTime() << "\n";

    const Queue& queue = controller.getQueue();
    vector<Customer*> queuedCustomers = queue.getSnapshot();
    cout << "Queue: ";
    if (queuedCustomers.empty()) {
        cout << "empty\n";
    } else {
        cout << "[";
        for (size_t i = 0; i < queuedCustomers.size(); i++) {
            Customer* customer = queuedCustomers[i];
            cout << "C" << customer->getCustomerNumber()
                 << "(w=" << customer->getWaitingTime() << ")";
            if (i + 1 < queuedCustomers.size()) {
                cout << ", ";
            }
        }
        cout << "]\n";
    }

    const vector<Server*>& servers = controller.getServers().getServers();
    for (size_t i = 0; i < servers.size(); i++) {
        const Server* server = servers[i];
        Customer* customer = server->getCurrentCustomer();
        cout << "Server " << (i + 1) << ": ";
        if (customer == nullptr) {
            cout << "idle\n";
        } else {
            cout << "serving C" << customer->getCustomerNumber()
                 << " (remaining=" << customer->getTransactionTime() << ")\n";
        }
    }
}

void ConsoleUI::displayMetrics() const {
    cout << "\n--- Final Metrics ---\n";
    cout << "Total customers served: " << controller.getTotalCustomersServed() << "\n";
    cout << "Total waiting time: " << controller.getTotalWaitingTime() << "\n";
    cout << fixed << setprecision(2);
    cout << "Average waiting time: " << controller.getAverageWaitingTime() << "\n";
    cout << "Average queue length: " << controller.getAverageQueueLength() << "\n";
    cout << "Server utilization: " << (controller.getServerUtilization() * 100.0) << "%\n";
    cout.unsetf(ios::floatfield);
}