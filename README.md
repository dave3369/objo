#  WPC (Whey Protein Concentrate) Factory Simulation

OOP with C++ | GIST EECS

##  1. Project Overview

This project is an interactive factory automation simulation implemented using C++ Object-Oriented Programming (OOP). We selected the Whey Protein Concentrate (WPC) manufacturing process as our target domain. Through an intuitive GUI built with Dear ImGui, users can control and monitor the production flow and the real-time status of the machines.

This simulation is fundamentally architected around the four core principles of OOP: abstraction, encapsulation, inheritance, and polymorphism. As a result, it ensures high maintainability and scalability, even within a complex and highly interactive system.

---

##  2. Core OOP Architecture (Design Justification)

### 1)  Polymorphism & Hierarchy
- The base class Machine is defined as an abstract class, and the common behavior update() is implemented using the Template Method Pattern. Within the main simulation loop (FactorySimulation::runTick()), the system iterates over an array of base class pointers (vector<Machine*>) and polymorphically calls m->update(), completely without the need for any if/else branching or dynamic_cast to identify the concrete type of each machine (Ultrafiltration, Dryer, Packaging).

### 2) Open-Closed Principle)
- When adding a new processing machine, you only need to define a new subclass that inherits from the Machine class and override the specific process logic (process()) and breakdown probability (breakdownChance()). Since both the main loop and the ImGui UI pipeline code rely exclusively on the abstracted Machine interface, the system allows for limitless functional expansion while completely preserving the core backend engine and UI code.

### 3) Strict Encapsulation
- The member variables of the Machine class and all simulation-related classes (such as status, hp, processingTime, and inputBuffer) are restricted to private or protected for subclass access. There are exactly zero public data members accessible to a code reviewer. State changes are strictly driven only through encapsulated interface methods (such as receiveItem(), repair(), and forceBreak()), ensuring complete data integrity.

### 4) UI & Backend Decoupling
- The backend core classes (such as Machine and ItemBuffer) do not contain a single line of Dear ImGui-related headers or UI dependencies. Instead, the MachineController structure was introduced to prevent the UI layer from directly corrupting the state of the backend domain objects. Only the top-level entry point, main.cpp, is aware of both sides. It converts UI button click events into refined, abstracted methods and safely delivers them in accordance with the timing rules of the backend engine.

##  3. Factory Domain Scenario

The simulation processes liquid whey into a finished powder product through a core three-stage pipeline. Each machine has its own unique processing time and breakdown probability.

### 1) Ultrafiltration 
* **Role:** Filters out water and lactose from the liquid whey to concentrate the protein (Processing time: 3 Ticks).
* **Feature:** Prone to intermittent breakdowns due to filter aging (Breakdown probability: 10%).

### 2) Spray Dryer - Bottleneck
* **Role:** Sprays hot air onto the concentrate to convert it into powder (Processing time: 7 Ticks).
* **Feature:** Has the longest processing time, inducing a natural bottleneck. It also has the highest breakdown probability (20%) due to temperature sensor issues.

### 3) Packaging 
* **Role:** Seals the protein powder into 2kg containers (Processing time: 2 Ticks).
* **Feature:** Has the fastest processing speed and does not break down (Breakdown probability: 0%).

---

##  4. Simulation Scenarios

During runtime, you can switch between the following scenarios in real-time using the ImGui dropdown menu.

1) Normal Flow : The pipeline runs at its default speed. Due to the differences in processing times between each machine (3 -> 7 -> 2), you can observe a realistic bottleneck where a queue of raw materials builds up in front of the second stage (the Dryer).
 
2) Random Breakdowns : The breakdown probability and the damage taken by the machines increase significantly. In this extreme environment, machines shut down in a chain reaction, allowing you to observe how the work-in-progress (WIP) accumulates and causes an overflow.

---

##  5. ImGui User Interface

1) Simulation Control: Provides Start, Pause, and Reset buttons, a simulation speed slider (1x-5x), a scenario selector, and a live Tick counter.

2) Factory Floor: Displays the current state of all machines in the process (WORKING, BROKEN, IDLE) at a glance using color-coded text.

3) Inspector: Clicking on a specific machine displays detailed information including its health bar, progress bar, and queue depth. Administrators can directly intervene using the Force Break and Instant Repair buttons.

4) Event Log: Records all events—such as machine state changes, repairs, and forced breakdowns—along with timestamps (Ticks) in a scrollable window (includes a Clear Log function).

5) Statistics: Tracks and aggregates the number of finished goods, the current work-in-progress (WIP) count, and total breakdowns in real-time.

---

##  6. How to Build & Run

1) Clone this repository.
2) Configure the build environment using CMake. (Requires integration with ImGui, SDL2, and OpenGL3.)
3) Run the built executable to start the simulation.

