#pragma once

#include <vector>
#include <deque>
#include <string>
#include "SimulationCore.h" // 💡 필수: SimulationLogger를 알기 위해 추가

class MachineController;

class FactoryView {
public:
    void renderControl(
        bool& simulationRunning,
        bool& resetRequested,
        int tick,
        int& speed,
        int& selectedScenario,
        const char* scenarios[],
        int scenarioCount
    );

    void renderFactoryFloor(
        const std::vector<MachineController>& controllers,
        int& selectedMachineIndex
    );

    // 💡 deque 대신 SimulationLogger 참조를 받도록 수정
    void renderInspector(
        const std::vector<MachineController>& controllers,
        int selectedMachineIndex,
        int tick, 
        SimulationLogger& logger 
    );

    // 💡 deque 대신 SimulationLogger 참조를 받도록 수정
    void renderEventLog(SimulationLogger& logger);

    void renderStatistics(
        int finishedGoods,
        int totalBreakdowns,
        const std::vector<MachineController>& controllers
    );
};