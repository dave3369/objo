#include "FactoryView.h"
#include "SimulationBridge.h" 
#include "imgui.h"

void FactoryView::renderControl(
    SimulationCommand& cmd,
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

    if (ImGui::Button("Start")) cmd.startRequested = true;
    ImGui::SameLine();
    if (ImGui::Button("Pause")) cmd.pauseRequested = true;
    ImGui::SameLine();
    if (ImGui::Button("Reset")) cmd.resetRequested = true;

    ImGui::SliderInt("Speed", &speed, 1, 5);

    ImGui::Combo("Scenario", &selectedScenario, scenarios, scenarioCount);

    ImGui::End();
}

void FactoryView::renderFactoryFloor(
    const std::vector<MachineSnapshot>& snapshots,
    int& selectedMachineIndex
) {
    ImGui::SetNextWindowPos(ImVec2(490, 60), ImGuiCond_Once);
    ImGui::SetNextWindowSize(ImVec2(310, 160), ImGuiCond_Once);

    ImGui::Begin("Factory Floor");

    ImGui::Text("Machines (click to inspect)");
    ImGui::Separator();

    for (int i = 0; i < snapshots.size(); i++) {
        const MachineSnapshot& snap = snapshots[i];

        bool selected = (selectedMachineIndex == i);

        if (ImGui::Selectable(snap.name.c_str(), selected)) {
            selectedMachineIndex = i;
        }

        ImGui::SameLine();

        if (snap.status == "Working" || snap.status == "WORKING") {
            ImGui::TextColored(ImVec4(0, 1, 0, 1), "Working"); // 연두색
        } else if (snap.status == "Broken" || snap.status == "BROKEN") {
            ImGui::TextColored(ImVec4(1, 0, 0, 1), "Broken");  // 빨간색
        } else {
            ImGui::Text("%s", snap.status.c_str());
        }
    }

    ImGui::End();
}

void FactoryView::renderInspector(
    const std::vector<MachineSnapshot>& snapshots,
    int selectedMachineIndex,
    int tick,
    SimulationCommand& cmd
) {
    ImGui::SetNextWindowPos(ImVec2(470, 250), ImGuiCond_Once);
    ImGui::SetNextWindowSize(ImVec2(380, 340), ImGuiCond_Once);

    ImGui::Begin("Inspector");

    if (!snapshots.empty() && selectedMachineIndex < snapshots.size()) {
        const MachineSnapshot& snap = snapshots[selectedMachineIndex];

        ImGui::Text("Selected Machine: %s", snap.name.c_str());
        ImGui::Separator();

        if (snap.status == "Working" || snap.status == "WORKING") {
            ImGui::TextColored(ImVec4(0, 1, 0, 1), "Status: %s", snap.status.c_str());
        } else if (snap.status == "Broken" || snap.status == "BROKEN") {
            ImGui::TextColored(ImVec4(1, 0, 0, 1), "Status: %s", snap.status.c_str());
        } else {
            ImGui::Text("Status: %s", snap.status.c_str());
        }

        ImGui::Text("HP: %d", snap.hp);
        ImGui::Text("Current Item: %s", snap.currentItemName.c_str());

	if (snap.name == "Ultrafiltration") {
        ImGui::Text("Operation: filter raw milk");
        ImGui::Text("Output: Liquid Whey");
    }
    else if (snap.name == "Dryer") {
        ImGui::Text("Operation: dry liquid whey");
        ImGui::Text("Output: WPC Powder");
    }
    else if (snap.name == "Packaging") {
        ImGui::Text("Operation: package WPC powder");
        ImGui::Text("Output: Finished Product");
    }

        ImGui::ProgressBar(snap.hp / 100.0f, ImVec2(330, 20));

        float workProgress = 0.0f;
        if (snap.processingTime > 0) {
            workProgress = (float)snap.progress / snap.processingTime;
        }

        ImGui::Text("Progress: %d / %d", snap.progress, snap.processingTime);
        ImGui::ProgressBar(workProgress, ImVec2(330, 20));

        ImGui::Text("Queue Size: %d", snap.queueSize);
        ImGui::Text("Output Count: %d", snap.outputCount);
        ImGui::Text("Process Time: %d", snap.processingTime);

        ImGui::Separator();

        if (ImGui::Button("Force Break")) {
            cmd.forceBreakRequested = true;
            cmd.targetMachineIndex = selectedMachineIndex;
        }
        ImGui::SameLine();
        if (ImGui::Button("Instant Repair")) {
            cmd.instantRepairRequested = true;
            cmd.targetMachineIndex = selectedMachineIndex;
        }
    }

    ImGui::End();
}

void FactoryView::renderEventLog(const std::deque<std::string>& logs, SimulationCommand& cmd) {
    ImGui::SetNextWindowPos(ImVec2(860, 60), ImGuiCond_Once);
    ImGui::SetNextWindowSize(ImVec2(490, 320), ImGuiCond_Once);

    ImGui::Begin("Event Log");

    if (ImGui::Button("Clear Log")) {
        cmd.clearLogRequested = true;
    }

    ImGui::Separator();

    ImGui::BeginChild("LogScroll", ImVec2(0, 220), true);

   for (const std::string& log : logs) {
        ImGui::Text("%s", log.c_str());
    }

    ImGui::EndChild();

    ImGui::End();
}


void FactoryView::renderStatistics(
    int finishedGoods,
    int totalBreakdowns,
    int  lostProducts,
    const std::vector<MachineSnapshot>& snapshots
) {
    ImGui::SetNextWindowPos(ImVec2(860, 430), ImGuiCond_Once);
    ImGui::SetNextWindowSize(ImVec2(230, 170), ImGuiCond_Once);

    ImGui::Begin("Statistics");

    int wipCount = 0;
    for (const MachineSnapshot& snap : snapshots) {
        wipCount += snap.queueSize;

        if (snap.hasCurrentItem) {
            wipCount++;
        }
    }

    ImGui::Text("Finished Goods: %d", finishedGoods);
    ImGui::Text("WIP Count: %d", wipCount);
    ImGui::Text("Total Breakdowns: %d", totalBreakdowns);
    ImGui::Text("Lost Products: %d", lostProducts);
    ImGui::End();
}
