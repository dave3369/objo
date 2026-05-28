#pragma once

#include "machine.h"
#include <vector>
#include <deque>
#include <string>

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
        const std::vector<Machine*>& machines,
        int& selectedMachineIndex
    );

    void renderInspector(
        const std::vector<Machine*>& machines,
        int selectedMachineIndex,
        int tick,
        std::deque<std::string>& eventLogs
    );

    void renderEventLog(std::deque<std::string>& eventLogs);

    void renderStatistics(
        FactorySimulation* sim,
        const std::vector<Machine*>& machines
    );
};
