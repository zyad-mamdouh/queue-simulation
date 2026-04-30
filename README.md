# Queue Simulation System

## Overview

Queue Simulation System is a C++17 console application that simulates a multi-server waiting line using discrete time steps.

The program models customers arriving over time, waiting in a FIFO queue, being assigned to available servers, and completing service after a configurable service duration. It also reports basic performance metrics such as total served customers, average waiting time, average queue length, and server utilization.

The current implementation uses dynamic customer generation instead of a fixed hard-coded customer list.

## Features

- Discrete step-based simulation
- Dynamic customer generation during runtime
- Support for future customer arrivals
- FIFO queue behavior
- Multiple server support
- Automatic assignment of queued customers to available servers
- Per-customer waiting time and remaining service time tracking
- Interactive console menu
- Input validation for console choices
- Summary metrics after running multiple steps

## Architecture

The project uses a simple layered structure:

```text
src/
|-- Main.cpp
|-- presentation/
|   |-- ConsoleUI.h
|   `-- ConsoleUI.cpp
|-- application/
|   |-- SimulationController.h
|   `-- SimulationController.cpp
`-- domain/
    |-- enums/
    |   `-- ServerStatus.hpp
    |-- interfaces/
    |   `-- QueueInterface.h
    |-- model/
    |   |-- Customer.h
    |   |-- Customer.cpp
    |   |-- CustomerGenerator.h
    |   |-- CustomerGenerator.cpp
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

### `ConsoleUI`

Handles user interaction through the terminal.

Responsibilities:

- Display the application header and menu
- Read and validate user input
- Run one step or multiple simulation steps
- Display queue state, server state, and metrics
- Create the default number of servers at startup

The current console setup creates `2` servers.

### `SimulationController`

Acts as a thin application layer between the console UI and the simulation engine.

Responsibilities:

- Forward commands from the UI to `QueueSystem`
- Expose read-only simulation state to the UI

### `QueueSystem`

The core simulation engine.

Responsibilities:

- Own the queue, server list, customer generator, and pending arrivals
- Advance the simulation one step at a time
- Move arrived customers into the queue
- Assign queued customers to available servers
- Update server processing
- Track performance metrics

### `CustomerGenerator`

Generates customers dynamically during simulation.

Current behavior:

- Uses `<random>` instead of `rand()`
- Uses a Poisson distribution for the number of generated customers per step
- Uses a uniform distribution for service time
- Uses a geometric distribution for arrival delay
- Can generate customers scheduled for future simulation times

Current default configuration in `QueueSystem`:

```cpp
generator(0.8, 1, 5, 3)
```

Meaning:

- average arrival rate: `0.8`
- minimum service time: `1`
- maximum service time: `5`
- maximum arrival delay: `3`

### `Queue`

Represents the FIFO waiting line.

Responsibilities:

- Enqueue arriving customers
- Dequeue customers for service
- Increment waiting time for customers still waiting
- Provide a snapshot for display

### `Server`

Represents one service station.

Responsibilities:

- Serve one customer at a time
- Track whether it is free or busy
- Reduce the current customer's remaining service time each step
- Become free when service completes

### `ServerList`

Manages all servers.

Responsibilities:

- Store servers
- Find the first available server
- Update all servers during a simulation step

### `Customer`

Represents a single customer.

Stores:

- customer number
- arrival time
- waiting time
- remaining service time

## Simulation Step Order

Each call to `runSimulationStep()` follows this order:

1. Generate new customers.
2. Store generated customers in `pendingArrivals`.
3. Move customers whose arrival time has arrived into the queue.
4. Assign queued customers to free servers before processing.
5. Update all busy servers for one time unit.
6. Assign queued customers again if a server became free after processing.
7. Increment waiting time for customers still left in the queue.
8. Record queue length and advance simulation time.

This means a server that finishes a customer during a step may immediately receive another waiting customer in the same step.

## Pending Arrivals

The system keeps a `pendingArrivals` list because generated customers may have future arrival times.

For example, a customer may be generated at time `4` with an arrival time of `6`. That customer should not enter the FIFO queue until simulation time reaches `6`.

## Metrics

The console reports:

- `Total customers served`
- `Total waiting time`
- `Average waiting time`
- `Average queue length`
- `Server utilization`

Server utilization is calculated from busy server time divided by total available server capacity.

## Build

This project does not currently use CMake or another build system. Compile manually with `g++`.

From the project root:

```bash
g++ -std=c++17 -Isrc -Isrc/domain/model -Isrc/domain/enums \
  src/Main.cpp \
  src/application/SimulationController.cpp \
  src/presentation/ConsoleUI.cpp \
  src/domain/model/Customer.cpp \
  src/domain/model/CustomerGenerator.cpp \
  src/domain/model/Queue.cpp \
  src/domain/model/Server.cpp \
  src/domain/model/ServerList.cpp \
  src/domain/system/QueueSystem.cpp \
  -o queue_sim
```

Run:

```bash
./queue_sim
```

## Console Usage

When the program starts, it displays an interactive menu:

```text
1  Run one simulation step
2  Run multiple steps
0  Exit
```

Options:

- `1`: run exactly one simulation step and display the current state
- `2`: run multiple simulation steps and display metrics afterward
- `0`: exit the program

## Example Output

```text
+--------------------------------------+
| Step completed                       |
+--------------------------------------+
Time: 3
Queue (1):
   1. Customer 4 | waiting: 1 | service left: 3
Servers:
  Server 1 | serving customer 2 | service left: 2
  Server 2 | idle
```

## Current Design Notes

- The project is organized with a basic presentation, application, and domain split.
- Customer generation is now dynamic rather than hard-coded in the UI.
- The console display has been improved with clearer state and metric formatting.
- The code currently uses raw pointers for customers and servers. A future improvement would be to migrate ownership to `std::unique_ptr` to make memory management safer and clearer.

## Recent Changes

- Replaced hard-coded customer setup with dynamic `CustomerGenerator`.
- Added support for generated customers with future arrival times through `pendingArrivals`.
- Updated the simulation step order to generate arrivals, move arrived customers to the queue, assign customers before server updates, update servers, assign again after completed service, then increment waiting time.
- Added `assignCustomers()` in `QueueSystem` to keep server assignment logic reusable.
- Improved console output with a clearer header, menu, queue display, server display, and aligned metrics.
- Added validated numeric input for console menu choices.
- Updated the README to match the current project behavior and architecture.
