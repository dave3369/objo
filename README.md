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
- The backend core classes do not contain a single line of Dear ImGui-related headers. We completely eliminated intermediate controller classes by introducing `SimulationBridge.h`. The UI strictly reads pure data structs (`MachineSnapshot`) and dispatches pure action requests (`SimulationCommand`). This ensures a 100% decoupled MVC architecture where the View and the Model are entirely oblivious to each other's implementation.

---

## 3. Advanced Design Patterns

To ensure maximum scalability and compliance with the OCP, we implemented several advanced design patterns:

### 1) **Observer Pattern (Event System):** 
An `IObserver` interface was introduced. The `SimulationLogger` subscribes to machines, which broadcast decoupled notifications (e.g., `"BREAKDOWN"`, `"BREAKDOWN_ITEM_LOST"`) without knowing how the logs are rendered.

### 2) **Singleton & Strategy (RecipeManager):** 
To completely decouple machine logic from specific products, we introduced a `RecipeManager`. It registers functional transformation rules (`TransformerFunc`) at runtime. Machines no longer hardcode output types; they simply request a transformation (`transformItem()`).

### 3) **Builder Pattern (Pipeline Construction):** 
The `FactoryBuilder` provides a fluent interface (`addUltrafiltration(3).addDryer(7)...`) to dynamically construct, link (`setNextMachine`), and inject the machine pipeline into the simulation without modifying the core engine.

---

## 4. Factory Domain Scenario

The simulation processes `Raw Milk` into a finished `WPC Powder` product through a three-stage pipeline.

### 1) Ultrafiltration (Stage 1)
* **Input/Output:** Raw Milk -> Liquid Whey
* **Role:** Filters out water and lactose to concentrate the protein (Processing time: 3 Ticks).
* **Feature:** Prone to intermittent breakdowns (Breakdown probability: 1.0%).

### 2) Spray Dryer - Bottleneck (Stage 2)
* **Input/Output:** Liquid Whey -> WPC Powder
* **Role:** Sprays hot air onto the concentrate to convert it into powder (Processing time: 7 Ticks).
* **Feature:** Induces a natural bottleneck due to the longest processing time. It has the highest breakdown probability (2.4%) due to temperature sensor wear.

### 3) Packaging (Stage 3)
* **Input/Output:** WPC Powder -> (Finished Good)
* **Role:** Seals the powder into commercial containers (Processing time: 2 Ticks).
* **Feature:** The fastest and most stable process (Breakdown probability: 0.6%).

---

## 5. Simulation Scenarios

Users can switch between the following scenarios in real-time via the ImGui dropdown menu:

### 1) **Normal Flow:** 
The pipeline runs at its default speed. Due to the differences in processing times (3 -> 7 -> 2), users can observe a realistic bottleneck where WIP (Work-In-Progress) accumulates in front of the Dryer.

### 2) **Random Breakdowns (Overflow & Product Loss):** 
The dynamic breakdown probability activates. Machines take damage every tick they operate and eventually shut down (`status = "BROKEN"`). Crucially, any WIP item inside a breaking machine is permanently lost, triggering a `BREAKDOWN_ITEM_LOST` event and forcing operators to monitor queue overflows.

---

## 6. ImGui User Interface Features

### 1) **Simulation Control:** 
Start, Pause, Reset buttons; Speed slider (1x-5x); Scenario selector dropdown.

### 2) **Factory Floor:** 
Displays real-time status (`WORKING`, `BROKEN`, `IDLE`) of all machines using color-coded text, cleanly driven by backend `MachineSnapshot` structs.

### 3) **Inspector:** 
Displays detailed machine telemetry (HP bar, Progress bar, Current Item, Queue Size). Administrators can intervene using **Force Break** and **Instant Repair** commands.

### 4) **Event Log:** 
Powered by the Observer pattern, it records all state changes, product losses, and repairs with timestamps in a scrollable view.

### 5) **Statistics:** 
Real-time tracking of Finished Goods, WIP count, Total Breakdowns, and **Lost Products**.

---

## 7. Class Diagram (UML)
### 7.1 Overall Structure
```mermaid
classDiagram
    direction TB

    %% ==========================================
    %% 1. UI & Bridge Layer (데이터 단절 계층)
    %% ==========================================
    namespace UI_Bridge_Layer {
        class FactoryView {
            +renderControl(cmd, tick, speed, selectedScenario...)
            +renderFactoryFloor(snapshots, selectedMachineIndex)
            +renderInspector(snapshots, selectedMachineIndex, tick, cmd)
            +renderEventLog(logs, cmd)
            +renderStatistics(finishedGoods, breakdowns, lostProducts, snapshots)
        }
        
        class MachineSnapshot {
            <<struct>>
            +name: string
            +status: string
            +hp: int
            +queueSize: int
            +progress: int
            +processingTime: int
            +hasCurrentItem: bool
            +currentItemName: string
            +outputCount: int
        }
        
        class SimulationCommand {
            <<struct>>
            +startRequested: bool
            +pauseRequested: bool
            +resetRequested: bool
            +clearLogRequested: bool
            +forceBreakRequested: bool
            +instantRepairRequested: bool
            +targetMachineIndex: int
            +clear() void
        }
    }

    %% ==========================================
    %% 2. Core Management (시뮬레이션 조립 및 루프)
    %% ==========================================
    namespace Core_Management {
        class FactorySimulation {
            -vector~unique_ptr~Machine~~ machines
            -int finishedGoods
            -int totalBreakdowns
            -int itemCounter
            +start(builtPipeline) void
            +runTick() void
            +getMachines() vector~unique_ptr~Machine~~
            +getTotalLostProducts() int
        }
        
        class FactoryBuilder {
            -vector~unique_ptr~Machine~~ pipeline
            +addUltrafiltration(time, initStatus) FactoryBuilder
            +addDryer(time, initStatus) FactoryBuilder
            +addPackaging(time, initStatus) FactoryBuilder
            +buildPipeline() vector~unique_ptr~Machine~~
        }
    }

    %% ==========================================
    %% 3. Event System (Observer Pattern)
    %% ==========================================
    namespace Event_System {
        class IObserver {
            <<interface>>
            +onNotify(machineName, eventType)* void
        }
        
        class SimulationLogger {
            -deque~string~ eventLogs
            -int currentTick
            +setTick(tick) void
            +onNotify(machineName, eventType) void
            +addManualLog(message) void
            +getLogs() deque~string~
            +clearLog() void
        }
    }

    %% ==========================================
    %% 4. Factory Domain (공장 기계와 버퍼)
    %% ==========================================
    namespace Factory_Machines {
        class SimulationObject {
            <<abstract>>
            #vector~IObserver*~ observers
            +update(tick)* void
            +notifyObservers(eventType) void
            +getInfo()* string
            +addObserver(obs) void
        }
        
        class ItemBuffer {
            -queue~unique_ptr~Product~~ q
            +push(item) void
            +pop() unique_ptr~Product~
            +clear() void
            +getSize() int
        }
        
        class Machine {
            <<abstract>>
            #int hp
            #string status
            #int processingTime
            #int currentWorkTime
            #unique_ptr~Product~ currentItem
            #ItemBuffer inputBuffer
            #int outputCount
            #int lostProductsCount
            +update(tick) void
            +process()* void
            +transformItem() void
            +breakdown() void
            +getSnapshot() MachineSnapshot
        }
        
        class Ultrafilteration {
            +process() void
            +breakdownChance() int
        }
        
        class Dryer {
            +process() void
            +breakdownChance() int
        }
        
        class Packaging {
            +process() void
            +breakdownChance() int
        }
    }

    %% ==========================================
    %% 5. Product & Recipes (OCP)
    %% ==========================================
    namespace Product_Recipes {
        class RecipeManager {
            <<Singleton>>
            -map registry
            +getInstance() RecipeManager$
            +registerRecipe(machineName, inputName, transformer) void
            +transform(machineName, item) unique_ptr~Product~
        }
        
        class Product {
            <<abstract>>
            -int id
            +getId() int
            +getName()* string
        }
        
        class RawMilk { +getName() string }
        class LiquidWhey { +getName() string }
        class WPCPowder { +getName() string }
    }

    %% ==========================================
    %% 의존성 및 관계선 (선 꼬임 방지를 위한 최적화)
    %% ==========================================
    
    %% 1. 상속 관계 (위에서 아래로 뻗어나가도록 배치)
    SimulationObject <|-- Machine : Inherits
    Machine <|-- Ultrafilteration
    Machine <|-- Dryer
    Machine <|-- Packaging
    Product <|-- RawMilk
    Product <|-- LiquidWhey
    Product <|-- WPCPowder
    IObserver <|-- SimulationLogger : Implements

    %% 2. 소유 및 관리 (Composition & Aggregation)
    FactorySimulation *-- Machine : Manages
    Machine *-- ItemBuffer : Owns (inputBuffer)
    SimulationObject o-- IObserver : Notifies Events
    ItemBuffer o-- Product : Queues

    %% 3. 참조 및 통신 (Dependencies)
    FactoryView ..> MachineSnapshot : Reads
    FactoryView ..> SimulationCommand : Writes
    FactorySimulation ..> SimulationCommand : Executes Actions
    Machine ..> MachineSnapshot : Generates
    FactoryBuilder --> Machine : Instantiates Pipeline
    Machine --> RecipeManager : Requests Transformation
    RecipeManager ..> Product : Creates


```
### 7.2 UI/Backend Decoupling (Bridge Pattern Focus)
```mermaid
classDiagram
    direction LR

    %% ==========================================
    %% 1. UI Layer (렌더링 및 사용자 입력)
    %% ==========================================
    namespace UI_Layer_ImGui {
        class FactoryView {
            +renderControl(cmd, tick, speed, selectedScenario...)
            +renderFactoryFloor(snapshots, selectedMachineIndex)
            +renderInspector(snapshots, selectedMachineIndex, tick, cmd)
            +renderEventLog(logs, cmd)
            +renderStatistics(finishedGoods, breakdowns, lostProducts, snapshots)
        }
    }

    %% ==========================================
    %% 2. Data Bridge Layer (UI와 백엔드의 완벽한 단절)
    %% ==========================================
    namespace Data_Bridge {
        class MachineSnapshot {
            <<struct (Read-Only)>>
            +name: string
            +status: string
            +hp: int
            +queueSize: int
            +progress: int
            +processingTime: int
            +hasCurrentItem: bool
            +currentItemName: string
            +outputCount: int
        }
        
        class SimulationCommand {
            <<struct (Write-Only)>>
            +startRequested: bool
            +pauseRequested: bool
            +resetRequested: bool
            +clearLogRequested: bool
            +forceBreakRequested: bool
            +instantRepairRequested: bool
            +targetMachineIndex: int
            +clear() void
        }
    }

    %% ==========================================
    %% 3. Backend Engine (핵심 비즈니스 로직)
    %% ==========================================
    namespace Backend_Engine {
        class FactorySimulation {
            -vector~unique_ptr~Machine~~ machines
            +runTick() void
        }
        
        class Machine {
            <<abstract>>
            #string status
            #int hp
            +update(tick) void
            +getSnapshot() MachineSnapshot
        }
    }

    %% ==========================================
    %% 의존성 및 관계선 (Decoupling Flow)
    %% ==========================================
    
    %% UI는 순수 데이터만 읽고, 순수 명령만 내림
    FactoryView ..> MachineSnapshot : 1. Render from pure data
    FactoryView ..> SimulationCommand : 2. Dispatch user inputs
    
    %% 백엔드는 명령을 받아 실행하고, 스냅샷을 찍어 넘김
    FactorySimulation ..> SimulationCommand : 3. Execute requested actions
    Machine ..> MachineSnapshot : 4. Package state safely
    
    %% 백엔드 내부 소유 관계
    FactorySimulation *-- Machine : Manages
```
### 7.3 Core Domain Logic 
```mermaid
classDiagram
    direction TB

    %% ==========================================
    %% 1. Core Management (파이프라인 생성 및 실행 루프)
    %% ==========================================
    namespace Core_Management {
        class FactorySimulation {
            -vector~unique_ptr~Machine~~ machines
            -int finishedGoods
            -int totalBreakdowns
            -int itemCounter
            +start(builtPipeline) void
            +runTick() void
            +getMachines() vector~unique_ptr~Machine~~
            +getTotalLostProducts() int
        }
        
        class FactoryBuilder {
            -vector~unique_ptr~Machine~~ pipeline
            +addUltrafiltration(time, initStatus) FactoryBuilder
            +addDryer(time, initStatus) FactoryBuilder
            +addPackaging(time, initStatus) FactoryBuilder
            +buildPipeline() vector~unique_ptr~Machine~~
        }
    }

    %% ==========================================
    %% 2. Factory Machines (공장 기계 도메인)
    %% ==========================================
    namespace Factory_Machines {
        class SimulationObject {
            <<abstract>>
            +update(tick)* void
        }
        
        class ItemBuffer {
            -queue~unique_ptr~Product~~ q
            +push(item) void
            +pop() unique_ptr~Product~
            +clear() void
            +getSize() int
        }
        
        class Machine {
            <<abstract>>
            #int hp
            #string status
            #int processingTime
            #int currentWorkTime
            #unique_ptr~Product~ currentItem
            #ItemBuffer inputBuffer
            #int outputCount
            #int lostProductsCount
            +update(tick) void
            +process()* void
            +transformItem() void
            +breakdown() void
        }
        
        class Ultrafilteration {
            +process() void
            +breakdownChance() int
        }
        
        class Dryer {
            +process() void
            +breakdownChance() int
        }
        
        class Packaging {
            +process() void
            +breakdownChance() int
        }
    }

    %% ==========================================
    %% 3. Product & Recipes (아이템과 변환 OCP)
    %% ==========================================
    namespace Product_Recipes {
        class RecipeManager {
            <<Singleton>>
            -map registry
            +getInstance() RecipeManager$
            +registerRecipe(machineName, inputName, transformer) void
            +transform(machineName, item) unique_ptr~Product~
        }
        
        class Product {
            <<abstract>>
            -int id
            +getId() int
            +getName()* string
        }
        
        class RawMilk { +getName() string }
        class LiquidWhey { +getName() string }
        class WPCPowder { +getName() string }
    }

    %% ==========================================
    %% 의존성 및 관계선 (비즈니스 로직 흐름 최적화)
    %% ==========================================
    
    %% 1. 상속 관계 (다형성의 핵심)
    SimulationObject <|-- Machine : Inherits
    Machine <|-- Ultrafilteration
    Machine <|-- Dryer
    Machine <|-- Packaging
    
    Product <|-- RawMilk
    Product <|-- LiquidWhey
    Product <|-- WPCPowder

    %% 2. 소유 및 파이프라인 구성 (객체의 생명주기)
    FactorySimulation *-- Machine : Manages Pipeline
    FactoryBuilder --> Machine : Instantiates
    Machine *-- ItemBuffer : Owns (inputBuffer)
    ItemBuffer o-- Product : Queues

    %% 3. 제품 변환 흐름 (디커플링된 OCP 적용부)
    Machine --> RecipeManager : Requests Transformation
    RecipeManager ..> Product : Creates via TransformerFunc
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
