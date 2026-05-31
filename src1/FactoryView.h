#pragma once

#include <vector>
#include <deque>
#include <string>

// MachineController 클래스가 존재한다는 것만 알려줌 (전방 선언)
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

    // Machine* 대신 MachineController를 넘겨받도록 변경
    void renderFactoryFloor(
        const std::vector<MachineController>& controllers,
        int& selectedMachineIndex
    );

    void renderInspector(
        const std::vector<MachineController>& controllers,
        int selectedMachineIndex,
        int tick,
        std::deque<std::string>& eventLogs
    );

    void renderEventLog(std::deque<std::string>& eventLogs);

    // FactorySimulation* 의존성 제거, 필요한 수치만 직접 받음
    void renderStatistics(
        int finishedGoods,
        int totalBreakdowns,
        const std::vector<MachineController>& controllers
    );
};