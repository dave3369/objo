#pragma once
#include <string>

struct MachineSnapshot {
    std::string name;
    std::string status;
    int hp;
    int queueSize;
    int progress;
    int processingTime;
    bool hasCurrentItem;
    std::string currentItemName;
    int outputCount;
};

struct SimulationCommand{
    bool startRequested = false;
    bool pauseRequested = false;
    bool resetRequested = false;
    bool clearLogRequested = false;
    bool forceBreakRequested = false;
    bool instantRepairRequested = false;
    int targetMachineIndex = -1;
    void clear() {
        startRequested = false;
        pauseRequested = false;
        resetRequested = false;
        forceBreakRequested = false;
        instantRepairRequested = false;
        targetMachineIndex = -1;
        clearLogRequested = false;
    }
};