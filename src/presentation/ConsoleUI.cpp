#include "ConsoleUI.h"
#include <limits>
#include <iostream>
#include <vector>
#include <iomanip>

using namespace std;

void ConsoleUI::run() {
    for (int i = 0; i < 2; i++) {
        controller.addServer(new Server());
    }

    displayHeader();

    while (true) {
        displayMenu();
        int choice = readInt("Select option: ");

        if (choice == 1) {
            controller.runStep();
            displayState();
        } else if (choice == 2) {
            int steps = readInt("Number of steps: ");
            if (steps <= 0) {
                cout << "\nPlease enter a positive number of steps.\n";
                continue;
            }

            for (int i = 0; i < steps; i++) {
                controller.runStep();
                displayState();
            }
            displayMetrics();
        } else if (choice == 0) {
            cout << "\nSimulation closed.\n";
            break;
        } else {
            cout << "\nUnknown option. Please choose 1, 2, or 0.\n";
        }
    }
}

void ConsoleUI::displayHeader() const {
    cout << "\n";
    cout << "========================================\n";
    cout << "        Queue Simulation System\n";
    cout << "========================================\n";
    cout << "Dynamic customer generation is enabled.\n";
    cout << "Servers: 2\n";
}

void ConsoleUI::displayMenu() {
    cout << "\n";
    cout << "----------------------------------------\n";
    cout << " Current time: " << controller.getCurrentTime() << "\n";
    cout << "----------------------------------------\n";
    cout << "  1  Run one simulation step\n";
    cout << "  2  Run multiple steps\n";
    cout << "  0  Exit\n";
}

void ConsoleUI::displayState() {
    cout << "\n";
    cout << "+--------------------------------------+\n";
    cout << "| Step completed                       |\n";
    cout << "+--------------------------------------+\n";
    cout << "Time: " << controller.getCurrentTime() << "\n";

    displayQueue();
    displayServers();
}

void ConsoleUI::displayQueue() const {
    const Queue& queue = controller.getQueue();
    vector<Customer*> queuedCustomers = queue.getSnapshot();
    cout << "Queue (" << queuedCustomers.size() << "): ";

    if (queuedCustomers.empty()) {
        cout << "empty\n";
        return;
    }

    cout << "\n";
    for (size_t i = 0; i < queuedCustomers.size(); i++) {
        Customer* customer = queuedCustomers[i];
        cout << "  " << setw(2) << (i + 1) << ". "
             << "Customer " << customer->getCustomerNumber()
             << " | waiting: " << customer->getWaitingTime()
             << " | service left: " << customer->getTransactionTime()
             << "\n";
    }
}

void ConsoleUI::displayServers() const {
    const vector<Server*>& servers = controller.getServers().getServers();
    cout << "Servers:\n";

    for (size_t i = 0; i < servers.size(); i++) {
        const Server* server = servers[i];
        Customer* customer = server->getCurrentCustomer();

        cout << "  Server " << (i + 1) << " | ";
        if (customer == nullptr) {
            cout << "idle\n";
        } else {
            cout << "serving customer " << customer->getCustomerNumber()
                 << " | service left: " << customer->getTransactionTime()
                 << "\n";
        }
    }
}

void ConsoleUI::displayMetrics() const {
    cout << "\n";
    cout << "+--------------------------------------+\n";
    cout << "| Metrics                              |\n";
    cout << "+--------------------------------------+\n";
    cout << left << setw(28) << "Total customers served" << controller.getTotalCustomersServed() << "\n";
    cout << left << setw(28) << "Total waiting time" << controller.getTotalWaitingTime() << "\n";
    cout << fixed << setprecision(2);
    cout << left << setw(28) << "Average waiting time" << controller.getAverageWaitingTime() << "\n";
    cout << left << setw(28) << "Average queue length" << controller.getAverageQueueLength() << "\n";
    cout << left << setw(28) << "Server utilization" << (controller.getServerUtilization() * 100.0) << "%\n";
    cout.unsetf(ios::floatfield);
}

int ConsoleUI::readInt(const char* prompt) const {
    int value;

    while (true) {
        cout << prompt;
        if (cin >> value) {
            return value;
        }

        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "Invalid number. Try again.\n";
    }
}
