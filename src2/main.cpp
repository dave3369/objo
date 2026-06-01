#include "imgui.h"
#include "imgui_impl_sdl2.h"
#include "imgui_impl_opengl3.h"

#include <SDL.h>
#include <SDL_opengl.h>
#include <deque>

#include "machine.h"
#include "SimulationBridge.h"
#include "FactoryView.h"

int main(int argc, char* argv[])
{
    srand(static_cast<unsigned int>(time(nullptr)));
    SDL_Init(SDL_INIT_VIDEO);

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);

    SDL_Window* window = SDL_CreateWindow(
        "Factory Simulation",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        1000,
        700,
        SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE
    );

    SDL_GLContext gl_context = SDL_GL_CreateContext(window);
    SDL_GL_MakeCurrent(window, gl_context);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    ImGui_ImplSDL2_InitForOpenGL(window, gl_context);
    ImGui_ImplOpenGL3_Init("#version 130");

    bool running = true;

    std::unique_ptr<FactorySimulation> sim = std::make_unique<FactorySimulation>();
    sim->start(FactoryBuilder()
        .addUltrafiltration(3) // 3초 걸리는 여과기 추가
        .addDryer(7)           // 7초 걸리는 건조기 추가
        .addPackaging(2)       // 2초 걸리는 포장기 추가
        .buildPipeline());

    FactoryView view;

    bool simulationRunning = false;
    int tick = 0;
    int speed = 1;
    int lastFinishedGoods = 0;
    int selectedMachineIndex = 0;

    int selectedScenario = 0;
    const char* scenarios[] = { "Normal Flow", "Random Breakdowns" };
    SimulationLogger logger;
    bool resetRequested = false;
    for (const auto& machine : sim->getMachines()) {
    machine->addObserver(&logger);
    }
    double lastTickTime = ImGui::GetTime();

    SimulationCommand cmd;

    while (running)
    {
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            ImGui_ImplSDL2_ProcessEvent(&event);
            if (event.type == SDL_QUIT)
                running = false;
        }

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplSDL2_NewFrame();
        ImGui::NewFrame();

        // =========================
        // Simulation update
        // =========================
        double currentTime = ImGui::GetTime();
        double interval = 1.0 / speed;

        if (simulationRunning && currentTime - lastTickTime >= interval)
        {
            sim->runTick();
            tick++;

	    if (sim->getFinishedGoods() > lastFinishedGoods)
    	    {
        	logger.addManualLog("Finished goods completed");
        	lastFinishedGoods = sim->getFinishedGoods();
    	    }

            lastTickTime = currentTime;
        }


const auto& machines = sim->getMachines();
        std::vector<MachineSnapshot> snapshots;
        for (const auto& machine : machines) {
            snapshots.push_back(machine->getSnapshot()); 
        }

view.renderControl(
    cmd, 
    tick, 
    speed, 
    selectedScenario, 
    scenarios, 
    IM_ARRAYSIZE(scenarios)
);

view.renderFactoryFloor(snapshots, selectedMachineIndex);
view.renderInspector(snapshots, selectedMachineIndex, tick, cmd);
view.renderEventLog(logger.getLogs(), cmd);
view.renderStatistics(sim->getFinishedGoods(), sim->getTotalBreakdowns(), sim->getTotalLostProducts(), snapshots);

if (cmd.clearLogRequested) {
    logger.clearLog();
}

if (cmd.startRequested) simulationRunning = true;
if (cmd.pauseRequested) simulationRunning = false;
if (cmd.resetRequested)  resetRequested = true;

if (cmd.forceBreakRequested && cmd.targetMachineIndex >= 0) {
    sim->getMachines()[cmd.targetMachineIndex]->forceBreak();
    logger.addManualLog(snapshots[cmd.targetMachineIndex].name + " was force broken");
}
if (cmd.instantRepairRequested && cmd.targetMachineIndex >= 0) {
            sim->getMachines()[cmd.targetMachineIndex]->repair();
            logger.addManualLog(snapshots[cmd.targetMachineIndex].name + " was repaired");
        }
cmd.clear();
if (resetRequested)
{
            sim = std::make_unique<FactorySimulation>();
            
            // 💡 리셋 시점에도 빌더를 통해 파이프라인을 새롭게 재조립하여 주입
            sim->start(FactoryBuilder()
                .addUltrafiltration(3)
                .addDryer(7)
                .addPackaging(2)
                .buildPipeline());

            tick = 0;
            simulationRunning = false;
            selectedMachineIndex = 0;
            lastFinishedGoods = 0;
            lastTickTime = ImGui::GetTime();

            // 관찰자 재등록 및 모드 갱신
            for (const auto& machine : sim->getMachines()) {
                machine->addObserver(&logger);
            }
            for (const auto& machine : sim->getMachines()) {
                machine->setRandomBreakdownMode(selectedScenario == 1);
            }

            logger.setTick(0); 
            logger.clearLog(); 
            logger.addManualLog("Simulation reset"); 
            resetRequested = false;
        }

const auto& updatedMachines = sim->getMachines();

bool randomMode = (selectedScenario == 1);
for (const auto& machine : updatedMachines)
{
    machine->setRandomBreakdownMode(randomMode);
}





        ImGui::Render();

        glViewport(0, 0, 1000, 700);
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        SDL_GL_SwapWindow(window);
    }

    

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();

    SDL_GL_DeleteContext(gl_context);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
