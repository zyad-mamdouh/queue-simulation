# Queue Simulation System

## Overview

Queue Simulation System is a C++ console application that models a simple multi-server queue using discrete time steps. Customers arrive with a known arrival time and required transaction time, enter a FIFO queue, and are assigned to the first available server. The program allows you to run the simulation one step at a time or for multiple steps and then inspect queue activity and performance metrics.

This project is organized with a small layered structure:

- `presentation` handles console interaction
- `application` coordinates use cases
- `domain` contains the core queueing logic and entities

## Features

- Step-based queue simulation
- FIFO queue behavior
- Support for multiple servers
- Per-customer arrival time, waiting time, and transaction time tracking
- Console menu for interactive execution
- Performance metrics for service and waiting behavior
- Simple architecture that is easy to extend for coursework or practice

## How The Simulation Works

Each simulation step follows this order:

1. The system inspects the servers at the beginning of the step to collect utilization and completion metrics.
2. Busy servers process one unit of transaction time.
3. Customers already waiting in the queue have their waiting time incremented by one.
4. Customers whose `arrivalTime` is less than or equal to the current simulation time are added to the queue.
5. Free servers pull customers from the front of the queue.
6. The queue length is recorded, the step counter is updated, and simulation time advances.

### Important Behavior Notes

- The queue is strictly FIFO.
- A customer arriving in the current step is enqueued after waiting times are incremented, so newly arrived customers do not gain waiting time until a later step.
- Server utilization is measured from the state of the servers at the beginning of each step.
- Queue length is recorded after available servers take customers from the queue.

## Default Scenario

The current console program starts with:

- 2 servers
- Customer 1: arrival time `0`, transaction time `3`
- Customer 2: arrival time `0`, transaction time `5`
- Customer 3: arrival time `0`, transaction time `2`

These values are currently hard-coded in `src/presentation/ConsoleUI.cpp`.

## Project Structure

```text
queue-simulation-system/
|-- README.md
|-- queue_sim_completed.exe
`-- src/
    |-- Main.cpp
    |-- application/
    |   |-- SimulationController.h
    |   `-- SimulationController.cpp
    |-- presentation/
    |   |-- ConsoleUI.h
    |   `-- ConsoleUI.cpp
    `-- domain/
        |-- enums/
        |   `-- ServerStatus.hpp
        |-- interfaces/
        |   `-- QueueInterface.h
        |-- model/
        |   |-- Customer.h
        |   |-- Customer.cpp
        |   |-- Queue.h
        |   |-- Queue.cpp
        |   |-- Server.h
        |   |-- Server.cpp
        |   |-- ServerList.h
        |   `-- ServerList.cpp
        `-- system/
            |-- QueueSystem.h
            `-- QueueSystem.cpp
```

## Main Components

### `Customer`

Represents a customer in the system.

Stores:

- customer number
- arrival time
- waiting time
- remaining transaction time

Supports:

- increasing waiting time
- reducing remaining transaction time
- exposing customer data through getters

### `Queue`

Represents the waiting line using `std::queue<Customer*>`.

Supports:

- enqueueing customers
- dequeueing customers
- incrementing waiting time for all queued customers
- creating a snapshot for display in the UI

### `Server`

Represents one service station.

Supports:

- serving one current customer at a time
- reporting whether it is free or busy
- processing one simulation time step

### `ServerList`

Manages a collection of servers.

Supports:

- adding servers
- finding the first available server
- updating all servers during a simulation step

### `QueueSystem`

This is the core simulation engine.

Responsibilities:

- manage the queue and server list
- store pending customer arrivals
- advance time
- run one step or many steps
- calculate summary metrics

### `SimulationController`

Acts as a thin application layer between the UI and the domain logic.

### `ConsoleUI`

Provides the menu-driven interface and displays queue/server state and final metrics.

## Metrics

The program reports the following metrics:

- `Total customers served`
  Number of customers who completed service.
- `Total waiting time`
  Sum of waiting times for all completed customers.
- `Average waiting time`
  `totalWaitingTime / totalCustomersServed`
- `Average queue length`
  `totalQueueLength / stepsExecuted`
- `Server utilization`
  `totalBusyServerTime / (numberOfServers * stepsExecuted)`

If no customers have completed service yet, or if no steps have been executed, the corresponding averages remain `0.0`.

## Build Instructions

This repository does not currently include a build system such as CMake or a Visual Studio solution, so compilation is done manually.

### Requirements

- A C++17-compatible compiler
- Windows PowerShell or another terminal

The source code was verified in this environment with:

- `g++ 14.2.0`

### Compile With `g++`

Run this from the project root:

```powershell
g++ -std=c++17 -Isrc `
  src/Main.cpp `
  src/application/SimulationController.cpp `
  src/presentation/ConsoleUI.cpp `
  src/domain/model/Customer.cpp `
  src/domain/model/Queue.cpp `
  src/domain/model/Server.cpp `
  src/domain/model/ServerList.cpp `
  src/domain/system/QueueSystem.cpp `
  -o queue_sim.exe
```

Then run:

```powershell
.\queue_sim.exe
```

## Run The Bundled Executable

An already built executable is included in the repository:

```powershell
.\queue_sim_completed.exe
```

## Console Usage

When the program starts, you will see:

```text
1. Run Simulation Step
2. Run Simulation N Steps
0. Exit
Choice:
```

### Menu Options

- `1`
  Run exactly one simulation step and print the current queue/server state.
- `2`
  Run multiple steps, printing state after each step, then print final metrics.
- `0`
  Exit the application.

## Example Of Displayed State

After a step runs, the UI shows:

- current simulation time
- queued customers and their waiting times
- each server and the customer it is serving
- remaining transaction time for customers in service

Example format:

```text
--- Simulation Step Executed ---
Current time: 1
Queue: [C3(w=1)]
Server 1: serving C1 (remaining=2)
Server 2: serving C2 (remaining=4)
```

## Extending The Project

This codebase is a good base for adding more realistic simulation behavior. Possible next steps include:

- loading customers and servers from input files
- allowing users to add customers interactively
- replacing raw pointers with smart pointers
- adding automated tests
- adding a `CMakeLists.txt` file
- introducing random arrivals and service durations
- exporting results to CSV
- supporting priority queues or different dispatch strategies

## Current Limitations

- Initial customers and servers are hard-coded in the console UI.
- Memory is allocated with `new` and is not explicitly cleaned up.
- There is no automated test suite.
- There is no validation for invalid menu input.
- There is no external configuration or persistence.

## Summary

This project demonstrates the fundamentals of a discrete-time queue simulation using clean, understandable C++ classes. It is especially suitable for learning object-oriented design, practicing queueing concepts, or serving as a starting point for a more advanced simulation project.
