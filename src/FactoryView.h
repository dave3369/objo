#pragma once

#include <vector>
#include <deque>
#include <string>
#include "SimulationBridge.h" 

class FactoryView {
public:
    void renderControl(
        SimulationCommand& cmd, 
        int tick, 
        int& speed, 
        int& selectedScenario, 
        const char* scenarios[], 
        int scenarioCount
    );

    void renderFactoryFloor(
        const std::vector<MachineSnapshot>& snapshots,
        int& selectedMachineIndex
    );

    void renderInspector(
        const std::vector<MachineSnapshot>& snapshots,
        int selectedMachineIndex,
        int tick, 
        SimulationCommand& cmd 
    );

    void renderEventLog(const std::deque<std::string>& logs, SimulationCommand& cmd);

    
    void renderStatistics(
        int finishedGoods,
        int totalBreakdowns,
        int lostProducts,
        const std::vector<MachineSnapshot>& snapshots
    );
};
