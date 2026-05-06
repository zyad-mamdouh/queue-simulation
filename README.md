# Queue Simulation System

Queue Simulation System is a C++17 discrete-time queue simulator for a multi-server service system. Customers are generated over time, wait in a FIFO queue, get assigned to the first available server, and complete service after their transaction time reaches zero.

The current entry point runs a graphical Dear ImGui interface powered by SDL2 and OpenGL. A console UI is still present in the codebase and can be re-enabled from `src/Main.cpp` if needed.

## Quick Answer: Where Is `queue_sim_gui`?

`queue_sim_gui` is not source code. It is the compiled Linux executable produced by `g++`.

If the file does not exist after cloning the repository, build it from the project root:

```bash
git submodule update --init --recursive
```

```bash
g++ -std=c++17 \
  -Isrc \
  -Isrc/domain/model \
  -Isrc/domain/enums \
  -Iimgui \
  -Iimgui/backends \
  -I/usr/include/SDL2 \
  -D_REENTRANT \
  src/Main.cpp \
  src/application/SimulationController.cpp \
  src/presentation/GuiUI.cpp \
  src/domain/model/Customer.cpp \
  src/domain/model/CustomerGenerator.cpp \
  src/domain/model/Queue.cpp \
  src/domain/model/Server.cpp \
  src/domain/model/ServerList.cpp \
  src/domain/system/QueueSystem.cpp \
  imgui/imgui.cpp \
  imgui/imgui_draw.cpp \
  imgui/imgui_tables.cpp \
  imgui/imgui_widgets.cpp \
  imgui/backends/imgui_impl_sdl2.cpp \
  imgui/backends/imgui_impl_opengl3.cpp \
  -o queue_sim_gui \
  -lSDL2 \
  -lGL
```

Then run it:

```bash
./queue_sim_gui
```

## Requirements

On Ubuntu/Debian-based Linux:

```bash
sudo apt update
sudo apt install g++ libsdl2-dev libgl1-mesa-dev
```

You need:

- `g++` with C++17 support
- SDL2 development headers and library
- OpenGL development library
- the `imgui/` submodule initialized with `git submodule update --init --recursive`

## Features

- Graphical simulation view using Dear ImGui, SDL2, and OpenGL
- Step-by-step simulation controls
- Run-N-steps control
- Auto-run mode with configurable delay
- FIFO queue visualization
- Multiple server visualization
- Dynamic customer generation using random distributions
- Metrics panel with served customers, waiting time, queue length, and server utilization
- Console UI implementation kept in `src/presentation/ConsoleUI.*`
- Layered source organization: presentation, application, and domain

## How The Simulation Works

Each call to `QueueSystem::runSimulationStep()` does the following:

1. Generate new customers for the current time step.
2. Store generated customers in `pendingArrivals`; some may be scheduled for future arrival times.
3. Move customers whose arrival time is now due into the FIFO queue.
4. Assign waiting customers to available servers before service processing.
5. Let busy servers process one time unit.
6. Count customers whose service completes during the step.
7. Assign more waiting customers if servers became free.
8. Increment waiting time for customers still left in the queue.
9. Record queue length and advance simulation time.

Because assignment happens both before and after server processing, a server that finishes a customer during a step can immediately receive another waiting customer in the same step.

## Customer Generation

Customer generation is handled by `CustomerGenerator`.

The current default configuration is set in `QueueSystem`:

```cpp
generator(0.8, 1, 5, 3)
```

Meaning:

- average arrival rate: `0.8`
- minimum service time: `1`
- maximum service time: `5`
- maximum arrival delay: `3`

The generator uses:

- `std::poisson_distribution<int>` for number of arrivals per step
- `std::uniform_int_distribution<int>` for service time
- `std::geometric_distribution<int>` for arrival delay

## Project Structure

```text
queue-simulation-system/
|-- README.md
|-- src/
|   |-- Main.cpp
|   |-- application/
|   |   |-- SimulationController.cpp
|   |   `-- SimulationController.h
|   |-- presentation/
|   |   |-- ConsoleUI.cpp
|   |   |-- ConsoleUI.h
|   |   |-- GuiUI.cpp
|   |   `-- GuiUI.h
|   `-- domain/
|       |-- enums/
|       |   `-- ServerStatus.hpp
|       |-- interfaces/
|       |   `-- QueueInterface.h
|       |-- model/
|       |   |-- Customer.cpp
|       |   |-- Customer.h
|       |   |-- CustomerGenerator.cpp
|       |   |-- CustomerGenerator.h
|       |   |-- Queue.cpp
|       |   |-- Queue.h
|       |   |-- Server.cpp
|       |   |-- Server.h
|       |   |-- ServerList.cpp
|       |   `-- ServerList.h
|       `-- system/
|           |-- QueueSystem.cpp
|           `-- QueueSystem.h
`-- imgui/
    |-- imgui.cpp
    |-- imgui_draw.cpp
    |-- imgui_tables.cpp
    |-- imgui_widgets.cpp
    `-- backends/
        |-- imgui_impl_opengl3.cpp
        |-- imgui_impl_sdl2.cpp
        |-- imgui_impl_opengl3.h
        `-- imgui_impl_sdl2.h
```

## Main Components

### `GuiUI`

Runs the graphical application. It creates the SDL2 window, initializes ImGui/OpenGL, renders the queue scene, handles buttons and sliders, and displays metrics and logs.

### `ConsoleUI`

Provides a terminal-based menu for running one step or multiple steps. It is not the active entry point right now, but it remains available in the code.

### `SimulationController`

Acts as a small application layer between the UI and the simulation engine. It forwards commands and exposes read-only state for display.

### `QueueSystem`

Owns the queue, server list, customer generator, pending arrivals, simulation time, and metrics. This is the core domain service.

### `CustomerGenerator`

Creates customers dynamically during the simulation and can schedule arrivals for future simulation times.

### `Queue`

Wraps a FIFO `std::queue<Customer*>`, supports enqueue/dequeue operations, increments waiting times, and exposes snapshots for the UI.

### `Server`

Represents one service station. A server can serve one customer, process one unit of work per step, and become free when service is complete.

### `ServerList`

Stores all servers, finds the first available server, and updates all servers during each simulation step.

### `Customer`

Stores customer number, arrival time, waiting time, and remaining transaction time.

## Metrics

The UI reports:

- `Served`: number of customers who completed service
- `Tot.Wait`: total waiting time of completed customers
- `Avg Wait`: total waiting time divided by served customers
- `Avg Q`: average queue length over executed steps
- `Utilization`: busy server time divided by total server capacity

When no customers have completed service yet, averages safely remain `0.0`.

## Running The Console UI

The current `src/Main.cpp` starts `GuiUI`.

To run the console interface instead, change `src/Main.cpp` to include `ConsoleUI.h` and instantiate `ConsoleUI`:

```cpp
#include "presentation/ConsoleUI.h"

int main()
{
    ConsoleUI ui;
    ui.run();
    return 0;
}
```

Then build:

```bash
g++ -std=c++17 \
  -Isrc \
  -Isrc/domain/model \
  -Isrc/domain/enums \
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

## Current Limitations

- There is no CMake or Makefile yet, so builds are manual.
- The app uses raw pointers for customers and servers.
- There is no automated test suite.
- Runtime settings are not loaded from a config file.
- The GUI currently creates two servers by default.

## Suggested Next Steps

- Add a `CMakeLists.txt` file.
- Replace raw pointer ownership with `std::unique_ptr`.
- Add unit tests for `QueueSystem`, `Queue`, `Server`, and `CustomerGenerator`.
- Add configurable server count and generator parameters in the GUI.
- Export metrics to CSV.
- Add reset/restart support in the GUI.
