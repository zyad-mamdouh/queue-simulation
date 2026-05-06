# Code Explanation

## 1. 🧠 Big Picture
- This project is a C++ queue simulation system. It models customers arriving over time, waiting in a queue, being assigned to available servers, and leaving after their service time is finished.
- The problem it solves is the classic service-queue problem: given random customer arrivals and a fixed number of servers, the program simulates how the queue grows, how servers are used, and how long customers wait.
- When it runs, the current entry point starts a graphical user interface. The GUI lets the user run one simulation step, run many steps, or auto-run steps over time. It displays:
  - the waiting queue,
  - the two servers,
  - currently served customers,
  - simulation time,
  - metrics such as total served, total waiting time, average waiting time, average queue length, and server utilization.

The actual entry point is very small:

```cpp
int main() {
    GuiUI ui;
    ui.run();
    return 0;
}
```

The simulation has three main layers:

- **Presentation layer**: `GuiUI` and `ConsoleUI`, which let a user control and view the simulation.
- **Application layer**: `SimulationController`, which provides a simple API between the UI and the simulation engine.
- **Domain layer**: `QueueSystem`, `Queue`, `ServerList`, `Server`, `Customer`, and `CustomerGenerator`, which contain the actual queue logic.

## 2. 🗂️ File / Structure Overview
- `src/Main.cpp`: Program entry point; creates `GuiUI` and runs it.
- `src/application/SimulationController.h`: Declares the controller API used by the UI.
- `src/application/SimulationController.cpp`: Implements controller methods by forwarding calls to `QueueSystem`.
- `src/domain/system/QueueSystem.h`: Declares the full simulation engine and its metrics.
- `src/domain/system/QueueSystem.cpp`: Implements customer generation, arrivals, queueing, server assignment, server processing, and statistics.
- `src/domain/model/Customer.h`: Declares the `Customer` class and its fields.
- `src/domain/model/Customer.cpp`: Implements customer waiting-time and service-time behavior.
- `src/domain/model/Queue.h`: Declares the queue class that stores waiting customers.
- `src/domain/model/Queue.cpp`: Implements enqueue, dequeue, waiting-time updates, and queue snapshots.
- `src/domain/model/Server.h`: Declares the `Server` class.
- `src/domain/model/Server.cpp`: Implements server availability, assignment, and service processing.
- `src/domain/model/ServerList.h`: Declares a container for multiple servers.
- `src/domain/model/ServerList.cpp`: Implements server storage, server search, and per-step server updates.
- `src/domain/model/CustomerGenerator.h`: Declares the random customer generator.
- `src/domain/model/CustomerGenerator.cpp`: Implements random arrival counts, arrival delays, and service times.
- `src/domain/enums/ServerStatus.hpp`: Defines whether a server is `FREE` or `BUSY`.
- `src/domain/interfaces/QueueInterface.h`: Defines an abstract queue interface with `enqueue()` and `dequeue()`.
- `src/presentation/GuiUI.h`: Declares the Dear ImGui graphical interface.
- `src/presentation/GuiUI.cpp`: Implements the GUI, rendering, controls, logs, auto-run behavior, and SDL/OpenGL event loop.
- `src/presentation/ConsoleUI.h`: Declares an alternative console interface.
- `src/presentation/ConsoleUI.cpp`: Implements the text menu version of the simulation interface.
- `imgui/`: Vendored Dear ImGui library and backend/example files used by the graphical interface.
- `README.md`: Project documentation file.
- `queue_sim` and `queue_sim_gui`: Built executable files in the repository root.
- `imgui.ini`: Runtime ImGui layout/settings file.

Main classes:

- `Customer`: Represents one customer with an ID, arrival time, waiting time, and remaining transaction time.
- `Queue`: Stores customers waiting for service in FIFO order.
- `Server`: Serves one customer at a time.
- `ServerList`: Stores all servers and finds an available one.
- `CustomerGenerator`: Randomly creates new customers.
- `QueueSystem`: Owns the simulation state and performs each simulation step.
- `SimulationController`: Wraps `QueueSystem` for the UI.
- `GuiUI`: Runs and renders the graphical application.
- `ConsoleUI`: Runs and renders the console application.

## 3. 🔄 Execution Flow (Step by Step)
1. The program starts in `src/Main.cpp`.

```cpp
GuiUI ui;
ui.run();
```

2. A `GuiUI` object is created. It contains a `SimulationController`, and that controller contains a `QueueSystem`.

```cpp
class GuiUI {
private:
    SimulationController controller;
```

3. `GuiUI::run()` starts SDL video/timer support.

```cpp
if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) != 0) {
    fprintf(stderr, "SDL_Init: %s\n", SDL_GetError());
    return;
}
```

If SDL initialization fails, the GUI prints an error and exits the function.

4. The GUI configures an OpenGL 3.3 context, creates an SDL window, creates the OpenGL context, and enables vertical sync.

```cpp
SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
SDL_Window* window = SDL_CreateWindow(...);
SDL_GLContext gl_ctx = SDL_GL_CreateContext(window);
```

5. Dear ImGui is initialized and styled.

```cpp
IMGUI_CHECKVERSION();
ImGui::CreateContext();
ImGui::StyleColorsDark();
```

6. The GUI initializes the simulation with two servers.

```cpp
initServers();
addLog("Initialized. 2 servers ready.");
```

`initServers()` does this:

```cpp
controller.addServer(new Server());
controller.addServer(new Server());
```

7. The GUI enters its main loop.

```cpp
bool running = true;
while (running) {
    ...
}
```

Each loop iteration represents one rendered frame, not necessarily one simulation step.

8. SDL events are processed. If the user closes the window, `running` becomes `false`.

```cpp
while (SDL_PollEvent(&ev)) {
    ImGui_ImplSDL2_ProcessEvent(&ev);
    if (ev.type == SDL_QUIT) running = false;
}
```

9. If auto-run is enabled, the GUI checks how much real time has passed. When the configured delay has elapsed, it runs one simulation step.

```cpp
if (ms >= autoDelay) {
    controller.runStep();
    addLog(buf);
    lastAuto = Clock::now();
}
```

10. ImGui begins a new frame and draws the full-screen UI.

```cpp
ImGui_ImplOpenGL3_NewFrame();
ImGui_ImplSDL2_NewFrame();
ImGui::NewFrame();
```

11. The left sidebar is drawn. It contains controls and metrics.

```cpp
renderControlPanel(sideW - 4, ctrlH);
renderMetricsPanel(sideW - 4, metricsH);
```

12. The main visual scene is drawn. It shows the queue lane, queued people, arrows, servers, and current time.

```cpp
renderVisualScene(sceneW, totalH);
```

13. The frame is rendered using OpenGL and displayed in the SDL window.

```cpp
ImGui::Render();
glClear(GL_COLOR_BUFFER_BIT);
ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
SDL_GL_SwapWindow(window);
```

14. Whenever the user clicks **Run 1 Step**, **Run N Steps**, or auto-run triggers, the UI calls `SimulationController::runStep()`.

```cpp
void SimulationController::runStep() {
    system.runSimulationStep();
}
```

15. `QueueSystem::runSimulationStep()` performs the actual simulation work:

- generate new customers,
- move arrived customers into the queue,
- assign queued customers to free servers,
- process one unit of service time,
- count completed customers,
- assign again if servers became free,
- increment waiting time for customers still in the queue,
- update metrics,
- advance simulation time.

16. When the user closes the GUI window, the loop ends and the GUI shuts down ImGui, OpenGL, SDL, and the window.

```cpp
ImGui_ImplOpenGL3_Shutdown();
ImGui_ImplSDL2_Shutdown();
ImGui::DestroyContext();
SDL_GL_DeleteContext(gl_ctx);
SDL_DestroyWindow(window);
SDL_Quit();
```

## 4. 🔬 Deep Dive — Every Block Explained
### `src/Main.cpp`

```cpp
#include "presentation/GuiUI.h"
```

This includes the GUI class declaration so `main()` can create and run the graphical interface.

```cpp
int main() {
    GuiUI ui;
    ui.run();
    return 0;
}
```

- **What it does**: Starts the program.
- **Inputs**: None.
- **Returns**: `0`, meaning successful program termination.
- **Why it exists**: It is the required C++ entry point and selects the GUI interface as the active interface.

### `SimulationController`

```cpp
class SimulationController {
private:
    QueueSystem system;
```

- **What it does**: Owns one `QueueSystem`.
- **Why it exists**: It separates UI code from simulation engine code. The UI does not directly manipulate all internals of `QueueSystem`.

```cpp
SimulationController::SimulationController() = default;
```

- **What it does**: Uses the default constructor.
- **Inputs**: None.
- **Returns**: A constructed `SimulationController`.
- **Why it exists**: It lets the contained `QueueSystem` construct itself normally.

```cpp
void SimulationController::addCustomer(Customer* customer) {
    system.addCustomer(customer);
}
```

- **What it does**: Adds a customer to the simulation through `QueueSystem`.
- **Inputs**: A `Customer*`.
- **Returns**: Nothing.
- **Why it exists**: Gives the UI or other caller a simple way to manually add customers.

```cpp
void SimulationController::addServer(Server* server) {
    system.addServer(server);
}
```

- **What it does**: Adds a server to the simulation.
- **Inputs**: A `Server*`.
- **Returns**: Nothing.
- **Why it exists**: Allows setup of the server list.

```cpp
void SimulationController::runStep() {
    system.runSimulationStep();
}
```

- **What it does**: Runs exactly one simulation time step.
- **Inputs**: None.
- **Returns**: Nothing.
- **Why it exists**: This is the main action triggered by the UI.

```cpp
void SimulationController::runSimulation(int steps) {
    system.runSimulation(steps);
}
```

- **What it does**: Runs multiple simulation steps.
- **Inputs**: Number of steps.
- **Returns**: Nothing.
- **Why it exists**: Supports batch execution.

Getter methods such as:

```cpp
const Queue& SimulationController::getQueue() const {
    return system.getQueue();
}
```

- **What they do**: Return read-only references or values from `QueueSystem`.
- **Inputs**: None.
- **Returns**: Queue, servers, time, and metric values.
- **Why they exist**: The UI needs to display state without owning or recalculating the simulation.

### `QueueSystem`

```cpp
Queue queue;
ServerList serverList;
CustomerGenerator generator;
std::vector<Customer*> pendingArrivals;
int currentTime;
int totalCustomersServed;
long long totalWaitingTime;
long long totalQueueLength;
long long totalBusyServerTime;
int stepsExecuted;
```

- `queue`: Holds customers who have arrived and are waiting.
- `serverList`: Holds all servers.
- `generator`: Creates random new customers.
- `pendingArrivals`: Holds generated or manually added customers whose arrival time has not yet reached the current simulation time.
- `currentTime`: Current simulation time step.
- `totalCustomersServed`: Count of completed customers.
- `totalWaitingTime`: Sum of waiting times for completed customers.
- `totalQueueLength`: Sum of queue sizes measured once per step.
- `totalBusyServerTime`: Count of server-time units spent serving customers.
- `stepsExecuted`: Number of completed simulation steps.

Constructor:

```cpp
QueueSystem::QueueSystem()
    : generator(0.8, 1, 5, 3),
      currentTime(0),
      totalCustomersServed(0),
      totalWaitingTime(0),
      totalQueueLength(0),
      totalBusyServerTime(0),
      stepsExecuted(0)
      {}
```

- **What it does**: Initializes the simulation.
- **Inputs**: None.
- **Returns**: A constructed `QueueSystem`.
- **Why it exists**: Sets default simulation parameters and starts metrics at zero.
- The generator settings mean:
  - average arrival count uses arrival rate `0.8`,
  - service time is between `1` and `5`,
  - maximum arrival delay is `3`.

```cpp
void QueueSystem::addCustomer(Customer* customer) {
    if (customer != nullptr) {
        pendingArrivals.push_back(customer);
    }
}
```

- **What it does**: Stores a customer for future arrival processing.
- **Inputs**: `Customer*`.
- **Returns**: Nothing.
- **Why it exists**: Manual customers and generated customers are treated consistently through `pendingArrivals`.

```cpp
void QueueSystem::addServer(Server* server) {
    serverList.addServer(server);
}
```

- **What it does**: Adds a server to the server list.
- **Inputs**: `Server*`.
- **Returns**: Nothing.
- **Why it exists**: Lets the simulation have one or more service points.

Main step function:

```cpp
void QueueSystem::runSimulationStep() {
```

This is the heart of the simulation.

```cpp
int customersCompletingThisStep = 0;
long long waitingTimeFromCompletedCustomers = 0;
```

These local counters track completions only within this one step before adding them to the global metrics.

```cpp
std::vector<Customer*> generatedCustomers = generator.generate(currentTime);
for (Customer* customer : generatedCustomers) {
    pendingArrivals.push_back(customer);
}
```

- Randomly creates new customers.
- Customers may arrive now or in a future step depending on generated arrival delay.
- All generated customers go into `pendingArrivals` first.

```cpp
for (auto it = pendingArrivals.begin(); it != pendingArrivals.end();) {
    Customer* customer = *it;
    if (customer != nullptr && customer->getArrivalTime() <= currentTime) {
        queue.enqueue(customer);
        it = pendingArrivals.erase(it);
    } else {
        ++it;
    }
}
```

- Walks through pending arrivals.
- If a customer has arrived by the current time, they enter the waiting queue.
- The customer is erased from `pendingArrivals`.
- If the customer has not arrived yet, the iterator moves forward.

```cpp
assignCustomers();
```

- Assigns waiting customers to free servers before service processing.

```cpp
const std::vector<Server*>& servers = serverList.getServers();
for (Server* server : servers) {
    Customer* active = server->getCurrentCustomer();
    if (active != nullptr) {
        totalBusyServerTime++;
        if (active->getTransactionTime() <= 1) {
            customersCompletingThisStep++;
            waitingTimeFromCompletedCustomers += active->getWaitingTime();
        }
    }
}
```

- Looks at all busy servers before their service time is reduced.
- Each busy server contributes one unit to `totalBusyServerTime`.
- If a customer has `transactionTime <= 1`, that customer will finish during this step, so the code counts them as completing and adds their waiting time.

```cpp
serverList.updateServers();
```

- Tells every server to process one time unit.
- A server reduces its customer's transaction time.
- If the customer's transaction time reaches zero, the server becomes free.

```cpp
totalCustomersServed += customersCompletingThisStep;
totalWaitingTime += waitingTimeFromCompletedCustomers;
```

- Adds this step's completed-customer statistics to total statistics.

```cpp
assignCustomers();
```

- Runs assignment again after servers may have become free.
- This lets customers move from queue to servers in the same step after earlier customers finish.

```cpp
queue.incrementWaitingTimes();
```

- Every customer still waiting in the queue gains one unit of waiting time.
- Customers assigned to servers do not get this increment.

```cpp
totalQueueLength += queue.size();
stepsExecuted++;
currentTime++;
```

- Records queue size for average queue length.
- Counts the completed step.
- Advances simulation time.

Private assignment helper:

```cpp
void QueueSystem::assignCustomers() {
    while (!queue.isEmpty()) {
        Server* availableServer = serverList.findAvailableServer();
        if (availableServer == nullptr) {
            break;
        }
        availableServer->serve(queue.dequeue());
    }
}
```

- **What it does**: Moves customers from the queue into available servers.
- **Inputs**: None directly; uses `queue` and `serverList`.
- **Returns**: Nothing.
- **Why it exists**: Keeps assignment logic reusable because assignment happens twice per step.
- The loop stops when either:
  - the queue is empty, or
  - there are no free servers.

Multiple-step runner:

```cpp
void QueueSystem::runSimulation(int steps) {
    if (steps <= 0) {
        return;
    }
    for (int i = 0; i < steps; i++) {
        runSimulationStep();
    }
}
```

- **What it does**: Runs `steps` simulation steps.
- **Inputs**: Integer step count.
- **Returns**: Nothing.
- **Why it exists**: Supports batch simulation.
- If `steps <= 0`, it does nothing.

Metrics:

```cpp
double QueueSystem::getAverageWaitingTime() const {
    if (totalCustomersServed == 0) {
        return 0.0;
    }
    return static_cast<double>(totalWaitingTime) / totalCustomersServed;
}
```

- Avoids division by zero.
- Returns average waiting time only for completed customers.

```cpp
double QueueSystem::getAverageQueueLength() const {
    if (stepsExecuted == 0) {
        return 0.0;
    }
    return static_cast<double>(totalQueueLength) / stepsExecuted;
}
```

- Avoids division by zero.
- Returns average queue size across executed steps.

```cpp
double QueueSystem::getServerUtilization() const {
    const int serverCount = static_cast<int>(serverList.getServers().size());
    if (serverCount == 0 || stepsExecuted == 0) {
        return 0.0;
    }
    const long long totalServerCapacity = static_cast<long long>(serverCount) * stepsExecuted;
    return static_cast<double>(totalBusyServerTime) / totalServerCapacity;
}
```

- Avoids division by zero if there are no servers or no steps.
- Calculates the fraction of total server capacity that was actually busy.

### `Customer`

```cpp
class Customer {
private:
    int customerNumber;
    int arrivalTime;
    int waitingTime;
    int transactionTime;
```

- `customerNumber`: Unique customer label.
- `arrivalTime`: Time when the customer becomes eligible to enter the queue.
- `waitingTime`: Time spent waiting in the queue.
- `transactionTime`: Remaining service time.

```cpp
Customer::Customer(int customerNumber, int arrivalTime, int transactionTime)
    : customerNumber(customerNumber),
      arrivalTime(arrivalTime),
      waitingTime(0),
      transactionTime(transactionTime) {}
```

- **What it does**: Creates a customer.
- **Inputs**: Customer ID, arrival time, service duration.
- **Returns**: A constructed `Customer`.
- **Why it exists**: Stores customer-specific state.

```cpp
void Customer::incrementWaitingTime() {
    waitingTime++;
}
```

- **What it does**: Adds one time unit of queue waiting.
- **Inputs**: None.
- **Returns**: Nothing.
- **Why it exists**: Queue waiting is tracked per customer.

```cpp
void Customer::reduceTransactionTime() {
    if (transactionTime > 0) {
        transactionTime--;
    }
}
```

- **What it does**: Reduces remaining service time by one.
- **Inputs**: None.
- **Returns**: Nothing.
- **Why it exists**: Servers call this once per simulation step.
- The `if` prevents the value from going below zero.

Getter methods:

```cpp
int Customer::getWaitingTime() const;
int Customer::getTransactionTime() const;
int Customer::getCustomerNumber() const;
int Customer::getArrivalTime() const;
```

- **What they do**: Expose customer values safely without modifying the customer.
- **Inputs**: None.
- **Returns**: Individual integer fields.
- **Why they exist**: UI and simulation logic need to inspect customer state.

### `QueueInterface`

```cpp
class QueueInterface {
public:
    virtual ~QueueInterface() = default;
    virtual void enqueue(Customer* customer) = 0;
    virtual Customer* dequeue() = 0;
};
```

- **What it does**: Defines the minimum behavior of a queue.
- **Inputs**:
  - `enqueue()` takes a `Customer*`.
  - `dequeue()` takes no input.
- **Returns**:
  - `enqueue()` returns nothing.
  - `dequeue()` returns a `Customer*`.
- **Why it exists**: It gives `Queue` an abstract interface and allows queue behavior to be described independently from the concrete implementation.

### `Queue`

```cpp
class Queue : public QueueInterface {
private:
    std::queue<Customer*> customers;
```

- **What it does**: Stores waiting customers in first-in, first-out order.
- **Why it exists**: A service queue should serve the earliest waiting customer first.

```cpp
void Queue::enqueue(Customer* customer) {
    if (customer != nullptr) {
        customers.push(customer);
    }
}
```

- **What it does**: Adds a customer to the back of the queue.
- **Inputs**: `Customer*`.
- **Returns**: Nothing.
- **Why it exists**: Customers need a waiting area before assignment.
- Null customers are ignored.

```cpp
Customer* Queue::dequeue() {
    if (customers.empty()) return nullptr;

    Customer* front = customers.front();
    customers.pop();
    return front;
}
```

- **What it does**: Removes and returns the front customer.
- **Inputs**: None.
- **Returns**: Front `Customer*`, or `nullptr` if empty.
- **Why it exists**: Servers need to take the next waiting customer.

```cpp
void Queue::incrementWaitingTimes() {
    const int customerCount = static_cast<int>(customers.size());
    for (int i = 0; i < customerCount; i++) {
        Customer* front = customers.front();
        customers.pop();
        front->incrementWaitingTime();
        customers.push(front);
    }
}
```

- **What it does**: Increments waiting time for every customer currently in the queue.
- **Inputs**: None.
- **Returns**: Nothing.
- **Why it exists**: The underlying `std::queue` does not support direct iteration.
- The code rotates the queue: pop front, update it, push it back.
- `customerCount` is captured before the loop so each original queued customer is updated exactly once.

```cpp
std::vector<Customer*> Queue::getSnapshot() const {
    std::vector<Customer*> snapshot;
    std::queue<Customer*> copy = customers;
    while (!copy.empty()) {
        snapshot.push_back(copy.front());
        copy.pop();
    }
    return snapshot;
}
```

- **What it does**: Copies queue contents into a vector.
- **Inputs**: None.
- **Returns**: `std::vector<Customer*>`.
- **Why it exists**: UI code needs to display the queue without modifying the real queue.

```cpp
bool Queue::isEmpty() const {
    return customers.empty();
}

int Queue::size() const {
    return customers.size();
}
```

- **What they do**: Report queue state.
- **Inputs**: None.
- **Returns**: Empty/not empty and queue size.
- **Why they exist**: Assignment logic and metrics need these values.

### `ServerStatus`

```cpp
enum class ServerStatus {
    FREE,
    BUSY
};
```

- **What it does**: Represents whether a server can accept a new customer.
- **Why it exists**: It makes server state explicit and type-safe.

### `Server`

```cpp
Customer* currentCustomer;
ServerStatus status;
```

- `currentCustomer`: The customer currently being served, or `nullptr`.
- `status`: Whether the server is free or busy.

```cpp
Server::Server() : currentCustomer(nullptr), status(ServerStatus::FREE) {}
```

- **What it does**: Creates an idle server.
- **Inputs**: None.
- **Returns**: A constructed `Server`.
- **Why it exists**: Servers begin with no assigned customer.

```cpp
void Server::serve(Customer* customer) {
    if (customer == nullptr) return;

    currentCustomer = customer;
    status = ServerStatus::BUSY;
}
```

- **What it does**: Assigns a customer to the server.
- **Inputs**: `Customer*`.
- **Returns**: Nothing.
- **Why it exists**: This is how customers leave the queue and start service.
- Null customers are ignored.

```cpp
bool Server::isAvailable() const {
    return status == ServerStatus::FREE;
}
```

- **What it does**: Checks whether the server is free.
- **Inputs**: None.
- **Returns**: `true` if free, otherwise `false`.
- **Why it exists**: `ServerList` uses it to find a server for the next queued customer.

```cpp
void Server::processTimeStep() {
    if (currentCustomer == nullptr) return;

    currentCustomer->reduceTransactionTime();

    if (currentCustomer->getTransactionTime() == 0) {
        currentCustomer = nullptr;
        status = ServerStatus::FREE;
    }
}
```

- **What it does**: Processes one unit of service time.
- **Inputs**: None.
- **Returns**: Nothing.
- **Why it exists**: Each simulation step must advance service for busy servers.
- If there is no customer, it does nothing.
- If service reaches zero, the customer is removed from the server and the server becomes free.

```cpp
Customer* Server::getCurrentCustomer() const {
    return currentCustomer;
}
```

- **What it does**: Returns the currently served customer.
- **Inputs**: None.
- **Returns**: `Customer*`, or `nullptr`.
- **Why it exists**: UI and metrics logic need to know server state.

### `ServerList`

```cpp
std::vector<Server*> servers;
```

- Stores all servers as pointers.

```cpp
void ServerList::addServer(Server* server) {
    if (server != nullptr) {
        servers.push_back(server);
    }
}
```

- **What it does**: Adds a server.
- **Inputs**: `Server*`.
- **Returns**: Nothing.
- **Why it exists**: The simulation can contain multiple servers.
- Null servers are ignored.

```cpp
Server* ServerList::findAvailableServer() {
    for (auto server : servers) {
        if (server->isAvailable()) {
            return server;
        }
    }
    return nullptr;
}
```

- **What it does**: Finds the first free server.
- **Inputs**: None.
- **Returns**: A free `Server*`, or `nullptr`.
- **Why it exists**: Assignment needs to know where to send the next customer.
- The search is linear from the first server to the last server.

```cpp
void ServerList::updateServers() {
    for (auto server : servers) {
        server->processTimeStep();
    }
}
```

- **What it does**: Advances every server by one time step.
- **Inputs**: None.
- **Returns**: Nothing.
- **Why it exists**: The simulation step must update all servers.

```cpp
const std::vector<Server*>& ServerList::getServers() const {
    return servers;
}
```

- **What it does**: Returns a read-only reference to the server vector.
- **Inputs**: None.
- **Returns**: `const std::vector<Server*>&`.
- **Why it exists**: UI and metrics code need to inspect all servers.

### `CustomerGenerator`

```cpp
double arrivalRate;
int minServiceTime;
int maxServiceTime;
int maxArrivalDelay;
int nextCustomerNumber;
```

- `arrivalRate`: Average number of customers generated per step.
- `minServiceTime`: Minimum service duration.
- `maxServiceTime`: Maximum service duration.
- `maxArrivalDelay`: Maximum generated delay before arrival.
- `nextCustomerNumber`: ID assigned to the next customer.

Random distributions:

```cpp
std::poisson_distribution<int> arrivalCountDistribution;
std::uniform_int_distribution<int> serviceTimeDistribution;
std::geometric_distribution<int> arrivalDelayDistribution;
```

- Poisson distribution controls how many customers are generated each step.
- Uniform distribution controls service time.
- Geometric distribution controls arrival delay, capped by `maxArrivalDelay`.

Constructor:

```cpp
CustomerGenerator::CustomerGenerator(...)
    : arrivalRate(arrivalRate),
      minServiceTime(minServiceTime),
      maxServiceTime(maxServiceTime),
      maxArrivalDelay(maxArrivalDelay),
      nextCustomerNumber(1),
      randomEngine(std::random_device{}()),
      arrivalCountDistribution(arrivalRate),
      serviceTimeDistribution(minServiceTime, maxServiceTime),
      arrivalDelayDistribution(0.6) {
```

- **What it does**: Sets generator parameters and initializes random distributions.
- **Inputs**: Arrival rate, min service time, max service time, max arrival delay.
- **Returns**: A constructed `CustomerGenerator`.
- **Why it exists**: The simulation needs realistic random arrivals and service durations.

Validation:

```cpp
if (arrivalRate < 0.0) {
    throw std::invalid_argument("arrivalRate must be non-negative");
}
if (minServiceTime <= 0 || maxServiceTime < minServiceTime) {
    throw std::invalid_argument("invalid service time range");
}
if (maxArrivalDelay < 0) {
    throw std::invalid_argument("maxArrivalDelay must be non-negative");
}
```

- Rejects invalid generator settings.
- Prevents nonsensical distributions such as negative arrival rates or invalid service ranges.

```cpp
std::vector<Customer*> CustomerGenerator::generate(int currentTime) {
    std::vector<Customer*> customers;
    const int count = arrivalCountDistribution(randomEngine);

    for (int i = 0; i < count; i++) {
        const int arrivalTime = currentTime + generateArrivalDelay();
        const int serviceTime = generateServiceTime();
        customers.push_back(new Customer(nextCustomerNumber++, arrivalTime, serviceTime));
    }

    return customers;
}
```

- **What it does**: Generates zero or more customers for the current step.
- **Inputs**: Current simulation time.
- **Returns**: Vector of newly allocated `Customer*`.
- **Why it exists**: It is the source of dynamic customers.
- Each customer gets:
  - a unique number,
  - an arrival time,
  - a random service time.

```cpp
int CustomerGenerator::generateServiceTime() {
    return serviceTimeDistribution(randomEngine);
}
```

- **What it does**: Produces a random service time.
- **Inputs**: None.
- **Returns**: Integer service time.
- **Why it exists**: Keeps service-time generation separate from customer creation.

```cpp
int CustomerGenerator::generateArrivalDelay() {
    return std::min(arrivalDelayDistribution(randomEngine), maxArrivalDelay);
}
```

- **What it does**: Produces a random arrival delay and caps it.
- **Inputs**: None.
- **Returns**: Integer delay.
- **Why it exists**: Customers can be generated now but scheduled to arrive later.

### `GuiUI`

State fields:

```cpp
static constexpr int LOG_CAPACITY = 64;
SimulationController controller;
int stepsToRun = 5;
bool autoRun = false;
int autoDelay = 600;
char logLines[LOG_CAPACITY][128] = {};
int logHead = 0;
int logCount = 0;
```

- `LOG_CAPACITY`: Maximum number of stored log lines.
- `controller`: Access point to the simulation.
- `stepsToRun`: Number used by the "Run N Steps" control.
- `autoRun`: Whether automatic stepping is active.
- `autoDelay`: Delay in milliseconds between auto-run steps.
- `logLines`: Fixed-size circular log storage.
- `logHead`: Index where the next log entry will be written.
- `logCount`: Number of currently stored log entries.

Color palette:

```cpp
static const ImU32 COL_BG_SCENE   = IM_COL32( 14,  17,  23, 255);
static const ImU32 COL_FLOOR      = IM_COL32( 30,  35,  45, 255);
...
```

- Defines colors used when drawing the custom visual simulation scene.
- These are file-local constants, not class members.

```cpp
void GuiUI::addLog(const char* msg)
{
    snprintf(logLines[logHead], 128, "%s", msg);
    logHead = (logHead + 1) % LOG_CAPACITY;
    if (logCount < LOG_CAPACITY) logCount++;
}
```

- **What it does**: Adds one message to the circular log.
- **Inputs**: C string message.
- **Returns**: Nothing.
- **Why it exists**: The GUI shows recent user actions and auto-run events.
- When the log is full, new messages overwrite the oldest messages.

```cpp
void GuiUI::initServers()
{
    controller.addServer(new Server());
    controller.addServer(new Server());
}
```

- **What it does**: Adds two servers to the simulation.
- **Inputs**: None.
- **Returns**: Nothing.
- **Why it exists**: The GUI simulation starts with two service points.

```cpp
void GuiUI::drawPerson(...)
```

- **What it does**: Draws a stick-figure customer using ImGui draw-list primitives.
- **Inputs**:
  - draw list,
  - center position,
  - body color,
  - head color,
  - numeric label,
  - scale.
- **Returns**: Nothing.
- **Why it exists**: It visually represents customers in the queue and at servers.

Important block:

```cpp
char buf[8]; snprintf(buf, sizeof(buf), "%d", label);
ImVec2 ts = ImGui::CalcTextSize(buf);
dl->AddText({headC.x - ts.x * 0.5f, headC.y - ts.y * 0.5f},
            IM_COL32(10,10,20,255), buf);
```

- Converts the customer number to text.
- Centers the text inside the drawn head.

```cpp
void GuiUI::drawServer(...)
```

- **What it does**: Draws a server box.
- **Inputs**:
  - draw list,
  - top-left position,
  - width and height,
  - busy flag,
  - server index,
  - customer number,
  - time left.
- **Returns**: Nothing.
- **Why it exists**: It visually shows whether a server is idle or serving a customer.

When busy:

```cpp
if (busy) {
    drawPerson(...);
    ...
    float frac = 1.0f - (float)(timeLeft - 1) / 9.0f;
```

- Draws the served customer.
- Draws a progress-style bar based on `timeLeft`.
- The bar uses a capped visual formula rather than the customer's original total service time.

When idle:

```cpp
const char* idleTxt = "idle";
dl->AddText(..., idleTxt);
```

- Shows an idle label instead of a customer.

```cpp
void GuiUI::drawArrow(...)
```

- **What it does**: Draws a line and triangle arrowhead.
- **Inputs**: draw list, start point, end point, color, thickness.
- **Returns**: Nothing.
- **Why it exists**: It visually connects the queue to the servers.

```cpp
float len = sqrtf(dx*dx + dy*dy);
if (len < 1.0f) return;
```

- Avoids drawing an arrowhead for an extremely short arrow.

```cpp
void GuiUI::renderVisualScene(float width, float height)
```

- **What it does**: Draws the main queue/server visualization.
- **Inputs**: Available width and height.
- **Returns**: Nothing.
- **Why it exists**: This is the primary visual output of the GUI.

It creates a child window:

```cpp
ImGui::BeginChild("##scene", ImVec2(width, height), false,
                  ImGuiWindowFlags_NoScrollbar);
```

It draws a dark background:

```cpp
dl->AddRectFilled(origin, {origin.x + width, origin.y + height},
                  COL_BG_SCENE);
```

It calculates layout:

```cpp
float queueW      = width * 0.38f;
float arrowZoneW  = width * 0.10f;
float serversW    = width - queueW - arrowZoneW - margin * 2.0f;
```

- Queue takes 38% of the scene width.
- Arrow zone takes 10%.
- Servers use the remaining width.

It obtains the queue snapshot:

```cpp
const Queue& queue = controller.getQueue();
std::vector<Customer*> snap = queue.getSnapshot();
```

- The GUI reads queue state without modifying the real queue.

It draws visible queued people:

```cpp
for (int i = 0; i < (int)snap.size() && i < maxVisible; i++) {
    Customer* c = snap[i];
    float px = queueX + queueW - 30.0f - i * personStep;
    drawPerson(..., c->getCustomerNumber(), 1.0f);
}
```

- The first customer in the queue is drawn closest to the servers.
- If there are too many customers to fit, only the visible portion is drawn.

Overflow display:

```cpp
if ((int)snap.size() > maxVisible) {
    snprintf(more, sizeof(more), "+%d more", (int)snap.size() - maxVisible);
    dl->AddText(..., more);
}
```

- Shows how many customers are hidden when the queue is too long.

Empty queue display:

```cpp
if (snap.empty()) {
    const char* empty = "empty";
    dl->AddText(..., empty);
}
```

- Displays `empty` when nobody is waiting.

Server rendering:

```cpp
const std::vector<Server*>& servers = controller.getServers().getServers();
...
for (int i = 0; i < nServers; i++) {
    const Server* srv  = servers[i];
    Customer* cust = srv->getCurrentCustomer();
    drawServer(..., cust != nullptr, i,
               cust ? cust->getCustomerNumber() : 0,
               cust ? cust->getTransactionTime() : 0);
}
```

- Reads all servers.
- For each server, gets the current customer.
- Draws the server as busy or idle.

```cpp
void GuiUI::renderControlPanel(float width, float height)
```

- **What it does**: Draws controls for running the simulation.
- **Inputs**: Panel width and height.
- **Returns**: Nothing.
- **Why it exists**: Lets the user advance time manually or automatically.

Run one step button:

```cpp
if (ImGui::Button("Run 1 Step", ImVec2(-1, 34))) {
    controller.runStep();
    addLog(buf);
}
```

- Runs one simulation step.
- Logs the new time.

Run N steps button:

```cpp
ImGui::SliderInt("##s", &stepsToRun, 1, 100, "Steps: %d");
if (ImGui::Button("Run N Steps", ImVec2(-1, 34))) {
    for (int i = 0; i < stepsToRun; i++) controller.runStep();
    addLog(buf);
}
```

- User selects between 1 and 100 steps.
- Button runs that many individual steps.

Auto-run controls:

```cpp
if (autoRun) {
    if (ImGui::Button("Stop Auto-Run", ImVec2(-1, 34))) {
        autoRun = false; addLog("Auto-run stopped.");
    }
} else {
    if (ImGui::Button("Start Auto-Run", ImVec2(-1, 34))) {
        autoRun = true; addLog("Auto-run started.");
    }
}
```

- Shows either stop or start button depending on current auto-run state.

```cpp
ImGui::SliderInt("##d", &autoDelay, 100, 2000, "Delay: %d ms");
```

- Lets user choose auto-run delay from 100 to 2000 milliseconds.

```cpp
void GuiUI::renderMetricsPanel(float width, float height)
```

- **What it does**: Draws metrics and logs.
- **Inputs**: Panel width and height.
- **Returns**: Nothing.
- **Why it exists**: Shows calculated simulation statistics.

Metric row helper:

```cpp
auto row = [](const char* label, const char* val) {
    ImGui::TextDisabled("%s", label);
    ImGui::SameLine(150);
    ImGui::TextColored(..., "%s", val);
};
```

- Local lambda used to draw a label/value row consistently.

Metrics formatting:

```cpp
snprintf(buf,sizeof(buf),"%d",  controller.getTotalCustomersServed()); row("Served",   buf);
snprintf(buf,sizeof(buf),"%lld",controller.getTotalWaitingTime());      row("Tot.Wait", buf);
snprintf(buf,sizeof(buf),"%.2f",controller.getAverageWaitingTime());    row("Avg Wait", buf);
snprintf(buf,sizeof(buf),"%.2f",controller.getAverageQueueLength());    row("Avg Q",    buf);
```

- Converts numeric metrics into strings before rendering.

Utilization bar:

```cpp
double util = controller.getServerUtilization() * 100.0;
ImGui::ProgressBar((float)(util / 100.0), ImVec2(-1, 10), "");
```

- Converts utilization to percent for text.
- Converts it back to `0.0` to `1.0` for the progress bar.

Log rendering:

```cpp
int start = (logCount == LOG_CAPACITY) ? logHead : 0;
for (int i = 0; i < logCount; i++) {
    int idx = (start + i) % LOG_CAPACITY;
    ImGui::TextDisabled("%s", logLines[idx]);
}
```

- Displays log entries in chronological order.
- Uses wraparound when the circular buffer is full.

Main GUI run loop:

```cpp
void GuiUI::run()
```

- **What it does**: Creates the window, initializes rendering, handles events, draws frames, and shuts down.
- **Inputs**: None.
- **Returns**: Nothing.
- **Why it exists**: It is the main graphical application runtime.

### `ConsoleUI`

Although `Main.cpp` currently uses `GuiUI`, the console UI is also implemented.

State:

```cpp
SimulationController controller;
```

- The console UI has its own controller and simulation state.

```cpp
void ConsoleUI::run() {
    for (int i = 0; i < 2; i++) {
        controller.addServer(new Server());
    }
```

- **What it does**: Starts a text-menu simulation with two servers.
- **Inputs**: User input from `cin`.
- **Returns**: Nothing.
- **Why it exists**: Provides a terminal-based way to run the same simulation.

Main menu loop:

```cpp
while (true) {
    displayMenu();
    int choice = readInt("Select option: ");
```

- Repeatedly shows a menu and reads a choice.

Choice handling:

```cpp
if (choice == 1) {
    controller.runStep();
    displayState();
} else if (choice == 2) {
    ...
} else if (choice == 0) {
    break;
} else {
    cout << "\nUnknown option. Please choose 1, 2, or 0.\n";
}
```

- `1`: run one step.
- `2`: run multiple steps.
- `0`: exit.
- Anything else: print an error message.

```cpp
void ConsoleUI::displayHeader() const
```

- **What it does**: Prints title and basic setup.
- **Inputs**: None.
- **Returns**: Nothing.
- **Why it exists**: Gives the console interface an introductory header.

```cpp
void ConsoleUI::displayMenu()
```

- **What it does**: Prints current time and menu options.
- **Inputs**: None.
- **Returns**: Nothing.
- **Why it exists**: Allows the user to choose simulation actions.

```cpp
void ConsoleUI::displayState()
```

- **What it does**: Prints state after a completed step.
- **Inputs**: None.
- **Returns**: Nothing.
- **Why it exists**: Shows the queue and servers after simulation changes.

```cpp
void ConsoleUI::displayQueue() const {
    const Queue& queue = controller.getQueue();
    vector<Customer*> queuedCustomers = queue.getSnapshot();
```

- **What it does**: Prints waiting customers.
- **Inputs**: None.
- **Returns**: Nothing.
- **Why it exists**: Displays queue state without changing it.

For each queued customer:

```cpp
cout << "Customer " << customer->getCustomerNumber()
     << " | waiting: " << customer->getWaitingTime()
     << " | service left: " << customer->getTransactionTime()
     << "\n";
```

- Shows customer number, waiting time, and remaining service time.

```cpp
void ConsoleUI::displayServers() const
```

- **What it does**: Prints each server as idle or serving a customer.
- **Inputs**: None.
- **Returns**: Nothing.
- **Why it exists**: Shows server state in text form.

```cpp
void ConsoleUI::displayMetrics() const
```

- **What it does**: Prints total and average metrics.
- **Inputs**: None.
- **Returns**: Nothing.
- **Why it exists**: Gives summary statistics after multi-step runs.

```cpp
int ConsoleUI::readInt(const char* prompt) const
```

- **What it does**: Reads a valid integer from standard input.
- **Inputs**: Prompt text.
- **Returns**: Integer typed by the user.
- **Why it exists**: Protects menu input from non-number values.

Invalid input handling:

```cpp
cin.clear();
cin.ignore(numeric_limits<streamsize>::max(), '\n');
cout << "Invalid number. Try again.\n";
```

- Clears the failed input state.
- Discards the invalid line.
- Prompts the user again.

## 5. 🔗 How the Pieces Connect
- `Main.cpp` creates `GuiUI`.
- `GuiUI` owns a `SimulationController`.
- `SimulationController` owns a `QueueSystem`.
- `QueueSystem` owns:
  - `Queue`,
  - `ServerList`,
  - `CustomerGenerator`,
  - pending arrivals,
  - simulation metrics.
- `CustomerGenerator` creates `Customer` objects.
- `QueueSystem` places arrived customers into `Queue`.
- `QueueSystem::assignCustomers()` asks `ServerList` for a free `Server`.
- `ServerList::findAvailableServer()` checks each `Server` using `Server::isAvailable()`.
- A free `Server` receives a customer through `Server::serve(queue.dequeue())`.
- Each simulation step calls `ServerList::updateServers()`.
- `ServerList::updateServers()` calls `Server::processTimeStep()` on every server.
- `Server::processTimeStep()` calls `Customer::reduceTransactionTime()`.
- `Queue::incrementWaitingTimes()` calls `Customer::incrementWaitingTime()` for every still-waiting customer.
- `GuiUI` and `ConsoleUI` read state through `SimulationController` getters.

The central call chain for one GUI step is:

```cpp
GuiUI::renderControlPanel()
    -> SimulationController::runStep()
        -> QueueSystem::runSimulationStep()
            -> CustomerGenerator::generate()
            -> Queue::enqueue()
            -> QueueSystem::assignCustomers()
                -> ServerList::findAvailableServer()
                -> Queue::dequeue()
                -> Server::serve()
            -> ServerList::updateServers()
                -> Server::processTimeStep()
                    -> Customer::reduceTransactionTime()
            -> Queue::incrementWaitingTimes()
                -> Customer::incrementWaitingTime()
```

Data moves mostly as raw pointers:

```cpp
customers.push_back(new Customer(...));
pendingArrivals.push_back(customer);
queue.enqueue(customer);
availableServer->serve(queue.dequeue());
```

The same `Customer*` moves from generated-customer vector, to pending arrivals, to the queue, to a server.

## 6. ⚙️ Key Logic & Decisions
- **Null checks**: Several methods ignore null pointers.

```cpp
if (customer != nullptr) {
    customers.push(customer);
}
```

This appears in queue, server-list, and queue-system add methods.

- **Arrival decision**: A customer enters the queue when their arrival time is less than or equal to the current time.

```cpp
if (customer != nullptr && customer->getArrivalTime() <= currentTime) {
    queue.enqueue(customer);
}
```

- **Assignment loop**: Customers are assigned while there are waiting customers and available servers.

```cpp
while (!queue.isEmpty()) {
    Server* availableServer = serverList.findAvailableServer();
    if (availableServer == nullptr) {
        break;
    }
    availableServer->serve(queue.dequeue());
}
```

- **Server completion detection**: `QueueSystem` counts a customer as completing before calling `serverList.updateServers()` if the customer's remaining transaction time is `1` or less.

```cpp
if (active->getTransactionTime() <= 1) {
    customersCompletingThisStep++;
    waitingTimeFromCompletedCustomers += active->getWaitingTime();
}
```

Then `Server::processTimeStep()` reduces the transaction time and frees the server if it reaches zero.

- **Two assignment passes per step**: Assignment happens before and after server processing.
  - First pass fills any free servers before service advances.
  - Second pass fills servers that became free during this step.

```cpp
assignCustomers();
serverList.updateServers();
assignCustomers();
```

- **Waiting-time rule**: Waiting time is incremented after the second assignment pass.

```cpp
queue.incrementWaitingTimes();
```

This means only customers still in the queue after both assignment opportunities wait one more unit.

- **Queue rotation**: `Queue::incrementWaitingTimes()` temporarily pops and pushes each customer so it can access every item in a `std::queue`.

```cpp
Customer* front = customers.front();
customers.pop();
front->incrementWaitingTime();
customers.push(front);
```

- **Queue snapshot**: `Queue::getSnapshot()` copies the queue before reading it.

```cpp
std::queue<Customer*> copy = customers;
```

This preserves the real queue order and contents.

- **Random generation**:
  - customer count uses a Poisson distribution,
  - service time uses a uniform integer distribution,
  - arrival delay uses a geometric distribution capped by `maxArrivalDelay`.

```cpp
const int count = arrivalCountDistribution(randomEngine);
const int arrivalTime = currentTime + generateArrivalDelay();
const int serviceTime = generateServiceTime();
```

- **Metric division guards**: Average calculations return `0.0` when there is not enough data.

```cpp
if (totalCustomersServed == 0) {
    return 0.0;
}
```

```cpp
if (serverCount == 0 || stepsExecuted == 0) {
    return 0.0;
}
```

- **GUI auto-run timing**: Auto-run is based on real elapsed milliseconds, not frame count.

```cpp
int ms = (int)std::chrono::duration_cast<std::chrono::milliseconds>(
             now - lastAuto).count();
if (ms >= autoDelay) {
    controller.runStep();
}
```

- **GUI log as circular buffer**: When the log reaches 64 entries, it wraps around and overwrites old messages.

```cpp
logHead = (logHead + 1) % LOG_CAPACITY;
```

- **Console input validation**: The console interface loops until `cin >> value` succeeds.

```cpp
if (cin >> value) {
    return value;
}
```

## 7. 📌 Important Notes
- `Main.cpp` currently runs the GUI, not the console UI.

```cpp
GuiUI ui;
ui.run();
```

- The simulation starts with two servers in both GUI and console modes, but each UI initializes its own controller and its own separate simulation state.

```cpp
controller.addServer(new Server());
controller.addServer(new Server());
```

- Customers are dynamically generated every simulation step by `CustomerGenerator`; the user does not need to manually create customers during normal GUI execution.

- Generated customers may not arrive immediately. They can be placed in `pendingArrivals` first and only enter the queue when `arrivalTime <= currentTime`.

- The code uses raw pointers and `new` for customers and servers.

```cpp
customers.push_back(new Customer(nextCustomerNumber++, arrivalTime, serviceTime));
controller.addServer(new Server());
```

There is no visible deletion of those allocated objects in the project source, so object lifetime is managed implicitly by the process lifetime.

- `QueueSystem::assignCustomers()` is called twice in one step. This affects timing: a customer can be assigned to a server immediately after another customer finishes in the same step.

- A customer's waiting time only increases while they remain in the queue after both assignment passes. A customer generated and assigned immediately may have waiting time `0`.

- Average waiting time is based only on customers who completed service, not customers still waiting or currently being served.

```cpp
return static_cast<double>(totalWaitingTime) / totalCustomersServed;
```

- Average queue length is based on queue size sampled once per executed step, after assignment and waiting-time updates.

```cpp
totalQueueLength += queue.size();
```

- Server utilization is calculated as busy server time divided by total possible server time.

```cpp
const long long totalServerCapacity = static_cast<long long>(serverCount) * stepsExecuted;
return static_cast<double>(totalBusyServerTime) / totalServerCapacity;
```

- `Queue::getSnapshot()` returns the same customer pointers stored in the queue, but inside a separate vector. The vector is safe to iterate for display, but the customers themselves are still the actual simulation objects.

- `Queue::dequeue()` returns `nullptr` if called on an empty queue, but `QueueSystem::assignCustomers()` checks `!queue.isEmpty()` before calling it.

- The GUI creates an SDL window before checking whether `SDL_CreateWindow()` returned `nullptr`. If window creation failed, later SDL/OpenGL calls would receive a null window pointer.

- `CustomerGenerator` can throw `std::invalid_argument` if constructed with invalid parameters, but the current `QueueSystem` constructor uses valid constants:

```cpp
generator(0.8, 1, 5, 3)
```

- The GUI server progress bar is visual only. It does not know the original service time, so it estimates a fraction from remaining time:

```cpp
float frac = 1.0f - (float)(timeLeft - 1) / 9.0f;
```

- The vendored `imgui/` directory is external library code. The project uses ImGui headers and SDL/OpenGL backend files from it, but the queue simulation logic is in `src/`.
