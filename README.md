# WPC (Whey Protein Concentrate) Factory Simulation

OOP with C++ | GIST EECS

## 1. Project Overview

This project is an interactive factory automation simulation implemented using modern C++ Object-Oriented Programming (OOP). We selected the Whey Protein Concentrate (WPC) manufacturing process as our target domain. Through an intuitive GUI built with Dear ImGui, users can control and monitor the production flow and the real-time status of the machines.

This simulation is fundamentally architected around the four core principles of OOP (Abstraction, Encapsulation, Inheritance, Polymorphism) and strictly adheres to the **Open-Closed Principle (OCP)**. By completely decoupling the UI from the backend business logic and utilizing advanced design patterns, it ensures high memory safety, maintainability, and scalability within a highly interactive system.

---

## 2. Core OOP Architecture (Design Justification)

### 1) Polymorphism & Abstract Hierarchy
- The base class `Machine` inherits from `SimulationObject`. The main simulation loop (`FactorySimulation::runTick()`) iterates over an array of base class smart pointers (`std::unique_ptr<Machine>`) and polymorphically calls `m->update(tick)`. The system operates entirely without the need for `if/else` branching or `dynamic_cast` to identify the concrete type of each machine (`Ultrafilteration`, `Dryer`, `Packaging`).

### 2) Strict Encapsulation
- Internal state variables of the `Machine` and `ItemBuffer` classes (such as `status`, `hp`, `processingTime`, and queues) are strictly restricted to `private` or `protected`. There are exactly **zero public data members** accessible to a code reviewer. State changes are strictly driven through encapsulated interface methods ensuring complete data integrity.

### 3) Memory Safety (Modern C++)
- The entire pipeline and inventory system were refactored using `std::unique_ptr`. The `ItemBuffer` safely moves `unique_ptr<Product>`, preventing any memory leaks during high-speed simulations, forced breakdowns, or pipeline resets. 

### 4) UI & Backend Decoupling (MVC Approach)
- The backend core classes do not contain a single line of Dear ImGui-related headers. The UI strictly reads data through the `MachineController` wrapper and the `SimulationLogger`. The top-level entry point (`main.cpp`) bridges the two sides, securely transforming UI button clicks into abstracted backend commands.

---

## 3. Advanced Design Patterns

To ensure maximum scalability and compliance with the OCP, we implemented several advanced design patterns:

### 1) **Observer Pattern (Event System):** An `IObserver` interface was introduced. The `SimulationLogger` subscribes to machines, which broadcast decoupled notifications (`notifyObservers("BREAKDOWN")`) without knowing how the logs are rendered.
### 2) **Singleton & Strategy (RecipeManager):** To completely decouple machine logic from specific products, we introduced a `RecipeManager`. It registers functional transformation rules (`TransformerFunc`) at runtime. Machines no longer hardcode output types; they simply request a transformation (`transformItem()`).
### 3) **Builder Pattern (Pipeline Construction):** The `FactoryBuilder` provides a fluent interface (`addUltrafiltration(3).addDryer(7)...`) to dynamically construct, link (`setNextMachine`), and inject the machine pipeline into the simulation without modifying the core engine.

---

## 4. Factory Domain Scenario

The simulation processes `Raw Milk` into a finished `WPC Powder` product through a three-stage pipeline.

### 1) Ultrafiltration (Stage 1)
* **Input/Output:** Raw Milk -> Liquid Whey
* **Role:** Filters out water and lactose to concentrate the protein (Processing time: 3 Ticks).
* **Feature:** Prone to intermittent breakdowns (Breakdown probability: 5%).

### 2) Spray Dryer - Bottleneck (Stage 2)
* **Input/Output:** Liquid Whey -> WPC Powder
* **Role:** Sprays hot air onto the concentrate to convert it into powder (Processing time: 7 Ticks).
* **Feature:** Induces a natural bottleneck due to the longest processing time. It has the highest breakdown probability (12%) due to temperature sensor wear.

### 3) Packaging (Stage 3)
* **Input/Output:** WPC Powder -> (Finished Good)
* **Role:** Seals the powder into commercial containers (Processing time: 2 Ticks).
* **Feature:** The fastest and most stable process (Breakdown probability: 3%).

---

## 5. Simulation Scenarios

Users can switch between the following scenarios in real-time via the ImGui dropdown menu:

### 1) **Normal Flow:** The pipeline runs at its default speed. Due to the differences in processing times (3 -> 7 -> 2), users can observe a realistic bottleneck where WIP (Work-In-Progress) accumulates in front of the Dryer.
### 2) **Random Breakdowns:** The dynamic breakdown probability activates. Machines take damage every tick they operate and eventually shut down (`status = "Broken"`), forcing the operator to manually intervene and observe queue overflows.

---

## 6. ImGui User Interface Features

### 1) **Simulation Control:** Start, Pause, Reset buttons; Speed slider (1x-5x); Scenario selector dropdown.
### 2) **Factory Floor:** Displays real-time status (WORKING, BROKEN, IDLE) of all machines using color-coded text via `MachineController`.
### 3) **Inspector:** Displays detailed machine telemetry (HP bar, Progress bar, Current Item, Queue Size). Administrators can intervene using **Force Break** and **Instant Repair** buttons.
### 4) **Event Log:** Powered by the Observer pattern, it records all state changes and repairs with timestamps in a scrollable view.
- **Statistics:** Real-time tracking of Finished Goods, WIP count, and Total Breakdowns.

---

## 7. Class Diagram (UML)

```mermaid
classDiagram
    %% Core Simulation Management
    namespace Manager {
        class FactorySimulation {
            -vector~unique_ptr~Machine~~ machines
            +start(builtPipeline) void
            +runTick() void
        }
        class FactoryBuilder {
            -vector~unique_ptr~Machine~~ pipeline
            +addUltrafiltration(time) FactoryBuilder
            +addDryer(time) FactoryBuilder
            +addPackaging(time) FactoryBuilder
            +buildPipeline() vector~unique_ptr~Machine~~
        }
    }

    %% Observer Pattern
    namespace Event_System {
        class IObserver {
            <<interface>>
            +onNotify(machineName, eventType)* void
        }
        class SimulationLogger {
            -deque~string~ eventLogs
            +onNotify(machineName, eventType) void
            +addManualLog(message) void
        }
    }

    %% Core Hierarchy & Machines
    namespace Core_Machines {
        class SimulationObject {
            <<abstract>>
            #vector~IObserver*~ observers
            +update(tick)* void
            +notifyObservers(eventType) void
        }
        class Machine {
            <<abstract>>
            #int hp
            #string status
            #ItemBuffer inputBuffer
            #unique_ptr~Product~ currentItem
            +update(tick) void
            +process()* void
            +transformItem() void
            +breakdown() void
        }
        class Ultrafilteration { +process() void }
        class Dryer { +process() void }
        class Packaging { +process() void }
    }

    %% OCP & Inventory
    namespace Product_Management {
        class RecipeManager {
            <<Singleton>>
            -map registry
            +registerRecipe(machineName, inputName, transformer) void
            +transform(machineName, item) unique_ptr~Product~
        }
        class Product {
            <<abstract>>
            -int id
            +getName()* string
        }
        class RawMilk
        class LiquidWhey
        class WPCPowder
    }

    %% Decoupled UI Wrapper
    class MachineController {
        -Machine& machine
        +getStatus() string
        +getHp() int
        +forceBreak() void
        +repair() void
    }

    %% Relationships
    FactorySimulation *-- Machine : Composition
    FactoryBuilder --> Machine : Creates
    SimulationObject <|-- Machine : Inheritance
    Machine <|-- Ultrafilteration
    Machine <|-- Dryer
    Machine <|-- Packaging
    
    SimulationObject o-- IObserver : Notifies
    IObserver <|-- SimulationLogger : Implements
    
    Machine --> RecipeManager : Requests Transformation
    Product <|-- RawMilk
    Product <|-- LiquidWhey
    Product <|-- WPCPowder
    
    MachineController --> Machine : Safely Wraps
```
## 8. How to Build & Run

### 1) Clone this repository
git clone <your-repository-url>
cd <repository-folder>

### 2) Build using CMake (Command Line / Developer Command Prompt)
mkdir build
cd build
cmake ..
cmake --build . --config Release

### 3) Run the executable
./Release/FactorySimulation.exe
