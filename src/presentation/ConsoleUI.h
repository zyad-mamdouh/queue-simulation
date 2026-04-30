#ifndef CONSOLE_UI_H
#define CONSOLE_UI_H

#include "../application/SimulationController.h"

class ConsoleUI {
private:
    SimulationController controller;

public:
    void run();

private:
    void displayHeader() const;
    void displayMenu();
    void displayState();
    void displayQueue() const;
    void displayServers() const;
    void displayMetrics() const;
    int readInt(const char* prompt) const;
};

#endif
