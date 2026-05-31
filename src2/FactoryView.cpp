#include "FactoryView.h"
#include "machineController.h" 
#include "imgui.h"


// 1. (생략했었던 기존 코드 복구) Control 렌더링
void FactoryView::renderControl(
    bool& simulationRunning,
    bool& resetRequested,
    int tick,
    int& speed,
    int& selectedScenario,
    const char* scenarios[],
    int scenarioCount
) {
    ImGui::SetNextWindowPos(ImVec2(30, 60), ImGuiCond_Once);
    ImGui::SetNextWindowSize(ImVec2(410, 400), ImGuiCond_Once);

    ImGui::Begin("Factory Simulation Control");

    ImGui::Text("Current Tick: %d", tick);

    if (ImGui::Button("Start")) simulationRunning = true;
    ImGui::SameLine();
    if (ImGui::Button("Pause")) simulationRunning = false;
    ImGui::SameLine();
    if (ImGui::Button("Reset")) resetRequested = true;

    ImGui::SliderInt("Speed", &speed, 1, 5);

    ImGui::Combo("Scenario", &selectedScenario, scenarios, scenarioCount);

    ImGui::End();
}

// 2. (의존성 제거됨) Factory Floor 렌더링
void FactoryView::renderFactoryFloor(
    const std::vector<MachineController>& controllers,
    int& selectedMachineIndex
) {
    ImGui::SetNextWindowPos(ImVec2(490, 60), ImGuiCond_Once);
    ImGui::SetNextWindowSize(ImVec2(310, 160), ImGuiCond_Once);

    ImGui::Begin("Factory Floor");

    ImGui::Text("Machines (click to inspect)");
    ImGui::Separator();

    for (int i = 0; i < controllers.size(); i++) {
        // 외부에서 받아온 컨트롤러를 그대로 사용
        const MachineController& controller = controllers[i];

        bool selected = (selectedMachineIndex == i);

        if (ImGui::Selectable(controller.getName().c_str(), selected)) {
            selectedMachineIndex = i;
        }

        ImGui::SameLine();

        if (controller.getStatus() == "WORKING") {
            ImGui::TextColored(ImVec4(0, 1, 0, 1), "WORKING");
        } else if (controller.getStatus() == "BROKEN") {
            ImGui::TextColored(ImVec4(1, 0, 0, 1), "BROKEN");
        } else {
            ImGui::Text("%s", controller.getStatus().c_str());
        }
    }

    ImGui::End();
}

// 3. (의존성 제거됨) Inspector 렌더링
void FactoryView::renderInspector(
    const std::vector<MachineController>& controllers,
    int selectedMachineIndex,
    int tick,
    SimulationLogger& logger
) {
    ImGui::SetNextWindowPos(ImVec2(470, 250), ImGuiCond_Once);
    ImGui::SetNextWindowSize(ImVec2(380, 340), ImGuiCond_Once);

    ImGui::Begin("Inspector");

    if (!controllers.empty() && selectedMachineIndex < controllers.size()) {
        MachineController controller = controllers[selectedMachineIndex];

        ImGui::Text("Selected Machine: %s", controller.getName().c_str());
        ImGui::Separator();

        if (controller.getStatus() == "WORKING") {
            ImGui::TextColored(ImVec4(0, 1, 0, 1), "Status: %s", controller.getStatus().c_str());
        } else if (controller.getStatus() == "BROKEN") {
            ImGui::TextColored(ImVec4(1, 0, 0, 1), "Status: %s", controller.getStatus().c_str());
        } else {
            ImGui::Text("Status: %s", controller.getStatus().c_str());
        }

        ImGui::Text("HP: %d", controller.getHp());
        ImGui::Text("Current Item: %s", controller.getCurrentItemName().c_str());
        ImGui::ProgressBar(controller.getHp() / 100.0f, ImVec2(330, 20));

        float workProgress = 0.0f;
        if (controller.getProcessingTime() > 0) {
            workProgress = (float)controller.getProgress() / controller.getProcessingTime();
        }

        ImGui::Text("Progress: %d / %d", controller.getProgress(), controller.getProcessingTime());
        ImGui::ProgressBar(workProgress, ImVec2(330, 20));

        ImGui::Text("Queue Size: %d", controller.getQueueSize());
        ImGui::Text("Process Time: %d", controller.getProcessingTime());

        ImGui::Separator();

        if (ImGui::Button("Force Break")) {
        controller.forceBreak();
        // 💡 중앙 로거에 수동 로그 기록
        logger.addManualLog(controller.getName() + " was force broken"); 
    }
    ImGui::SameLine();
    if (ImGui::Button("Instant Repair")) {
        controller.repair();
        logger.addManualLog(controller.getName() + " was repaired");
    }
    }

    ImGui::End();
}

// 4. (생략했었던 기존 코드 복구) Event Log 렌더링
void FactoryView::renderEventLog(SimulationLogger& logger) {
    ImGui::SetNextWindowPos(ImVec2(860, 60), ImGuiCond_Once);
    ImGui::SetNextWindowSize(ImVec2(490, 320), ImGuiCond_Once);

    ImGui::Begin("Event Log");

    if (ImGui::Button("Clear Log")) {
        logger.clearLog();
    }

    ImGui::Separator();

    ImGui::BeginChild("LogScroll", ImVec2(0, 220), true);

    for (const std::string& log : logger.getLogs()) {
        ImGui::Text("%s", log.c_str());
    }

    ImGui::EndChild();

    ImGui::End();
}

// 5. (의존성 제거됨) Statistics 렌더링
void FactoryView::renderStatistics(
    int finishedGoods,
    int totalBreakdowns,
    const std::vector<MachineController>& controllers
) {
    ImGui::SetNextWindowPos(ImVec2(860, 430), ImGuiCond_Once);
    ImGui::SetNextWindowSize(ImVec2(230, 150), ImGuiCond_Once);

    ImGui::Begin("Statistics");

    int wipCount = 0;
    for (const MachineController& controller : controllers) {
        wipCount += controller.getQueueSize();

        if (controller.hasCurrentItem()) {
            wipCount++;
        }
    }

    ImGui::Text("Finished Goods: %d", finishedGoods);
    ImGui::Text("WIP Count: %d", wipCount);
    ImGui::Text("Total Breakdowns: %d", totalBreakdowns);

    ImGui::End();
}