#include "imgui.h"
#include "view.h"
#include "machineController.h"
using namespace std;

SimulationView::SimulationView() {
    selectedMachineIndex = 0;
    selectedScenario = 0;
    speed = 1;
    simulationRunning = false;
    resetRequested = false;
    lastFinishedGoods = 0;
}
void SimulationView::render(FactoryController& factoryCtrl, int tick) {
    auto& machines = factoryCtrl.getMachineControllers();

    // 1. Control Window (시뮬레이션 제어)

    ImGui::Begin("Factory Simulation Control");
    ImGui::Text("Current Tick: %d", tick);

    if (ImGui::Button("Start")) simulationRunning = true;
    ImGui::SameLine();
    if (ImGui::Button("Pause")) simulationRunning = false;
    ImGui::SameLine();
    if (ImGui::Button("Reset")) resetRequested = true;

    ImGui::SliderInt("Speed", &speed, 1, 5);

    const char* scenarios[] = { "Normal Flow", "Random Breakdowns" };
    ImGui::Combo("Scenario", &selectedScenario, scenarios, IM_ARRAYSIZE(scenarios));
    
    ImGui::End();


    // 2. Factory Floor (공장 라인 뷰)
    ImGui::Begin("Factory Floor");
    ImGui::Text("Machines (click to inspect)");
    ImGui::Separator();

    for (int i = 0; i < machines.size(); i++) {
        auto& machine = machines[i];
        bool selected = (selectedMachineIndex == i);

        // 클릭 시 선택된 인덱스 업데이트
        if (ImGui::Selectable(machine.getName().c_str(), selected)) {
            selectedMachineIndex = i;
        }

        ImGui::SameLine();

        // 상태별 텍스트 색상 변경
        std::string status = machine.getStatus();
        if (status == "WORKING") {
            ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "WORKING");
        } else if (status == "BROKEN") {
            ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "BROKEN");
        } else {
            ImGui::Text("%s", status.c_str());
        }
    }
    ImGui::End();

    // 3. Inspector 
   
    ImGui::Begin("Inspector");
    if (!machines.empty() && selectedMachineIndex < machines.size()) {
        auto& selectedMachine = machines[selectedMachineIndex];

        ImGui::Text("Selected Machine: %s", selectedMachine.getName().c_str());
        ImGui::Separator();

        std::string status = selectedMachine.getStatus();
        if (status == "WORKING") {
            ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "Status: %s", status.c_str());
        } else if (status == "BROKEN") {
            ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "Status: %s", status.c_str());
        } else {
            ImGui::Text("Status: %s", status.c_str());
        }

        // 체력(HP) 바
        ImGui::Text("HP: %d", selectedMachine.getHp());
        ImGui::ProgressBar(selectedMachine.getHp() / 100.0f, ImVec2(220, 20));

        // 작업 진행률 바
        float workProgress = 0.0f;
        int procTime = selectedMachine.getProcessingTime();
        int curProgress = selectedMachine.getProgress();
        
        if (procTime > 0) {
            workProgress = (float)curProgress / procTime;
        }

        ImGui::Text("Progress: %d / %d", curProgress, procTime);
        ImGui::ProgressBar(workProgress, ImVec2(220, 20));

        ImGui::Text("Queue Size: %d", selectedMachine.getQueueSize());
        ImGui::Text("Process Time: %d", procTime);
        ImGui::Separator();

        // 조작 버튼 (Controller의 Action 메서드 호출)
        if (ImGui::Button("Force Break")) {
            selectedMachine.onForceBreakClicked();
            eventLogs.push_front("[Tick " + std::to_string(tick) + "] " + selectedMachine.getName() + " was force broken");
        }
        ImGui::SameLine();
        if (ImGui::Button("Instant Repair")) {
            selectedMachine.onRepairClicked();
            eventLogs.push_front("[Tick " + std::to_string(tick) + "] " + selectedMachine.getName() + " was repaired");
        }
    }
    ImGui::End();

  
    // 4. Event Log (이벤트 로그 창)
   
    ImGui::Begin("Event Log");
    if (ImGui::Button("Clear Log")) {
        eventLogs.clear();
    }
    ImGui::Separator();

    ImGui::BeginChild("LogScroll", ImVec2(0, 150), true);
    for (const std::string& log : eventLogs) {
        ImGui::Text("%s", log.c_str());
    }
    ImGui::EndChild();
    ImGui::End();

    // =========================
    // 5. Statistics (통계 창)
    // =========================
    ImGui::Begin("Statistics");
    
    // WIP(진행 중인 재고) 계산
    int wipCount = 0;
    for (auto& machine : machines) {
        wipCount += machine.getQueueSize();
        if (machine.getStatus() == "WORKING") { // 현재 아이템이 있으면 (대략적인 확인)
            wipCount++;
        }
    }

    ImGui::Text("Finished Goods: %d", factoryCtrl.getLastFinishedGoods());
    ImGui::Text("WIP Count: %d", wipCount);
    ImGui::Text("Total Breakdowns: %d", factoryCtrl.getTotalBreakdowns());
    ImGui::Text("Lost Products: %d", 0);
    ImGui::End();
};