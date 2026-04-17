#ifndef CONSOLE_UI_H
#define CONSOLE_UI_H

#include "../application/SimulationController.h"

class ConsoleUI {
private:
    SimulationController controller;

public:
    void run();

private:
    void displayMenu();
    void displayState();
    void displayMetrics() const;
};

#endif