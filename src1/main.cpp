#include "imgui.h"
#include "imgui_impl_sdl2.h"
#include "imgui_impl_opengl3.h"

#include <SDL.h>
#include <SDL_opengl.h>
#include <deque>

#include "machine.h"
#include "machineController.h"
#include "FactoryView.h"

int main(int argc, char* argv[])
{
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

    FactorySimulation* sim = new FactorySimulation();
    sim->start();

    FactoryView view;

    bool simulationRunning = false;
    int tick = 0;
    int speed = 1;
    int lastFinishedGoods = 0;
    int selectedMachineIndex = 0;

    int selectedScenario = 0;
    const char* scenarios[] = { "Normal Flow", "Random Breakdowns" };
    std::deque<std::string> eventLogs;
    bool resetRequested = false;

    double lastTickTime = ImGui::GetTime();

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
        	eventLogs.push_front(
            	"[Tick " + std::to_string(tick) +
            	"] Finished goods completed"
        	);

        	lastFinishedGoods = sim->getFinishedGoods();
    	    }

            lastTickTime = currentTime;
        }

        // =========================
        // UI
        // =========================

const auto& machines = sim->getMachines();

std::vector<MachineController> controllers;
for (Machine* machine : machines) {
    controllers.emplace_back(*machine); 
}

view.renderControl(
    simulationRunning,
    resetRequested,
    tick,
    speed,
    selectedScenario,
    scenarios,
    IM_ARRAYSIZE(scenarios)
);

if (resetRequested)
{
    delete sim;

    sim = new FactorySimulation();
    sim->start();

    tick = 0;
    simulationRunning = false;
    selectedMachineIndex = 0;
    lastFinishedGoods = 0;
    lastTickTime = ImGui::GetTime();

    for (Machine* machine : sim->getMachines())
    {
        machine->setRandomBreakdownMode(selectedScenario == 1);
    }

    eventLogs.push_front("[Tick 0] Simulation reset");

    resetRequested = false;
}

const auto& updatedMachines = sim->getMachines();

bool randomMode = (selectedScenario == 1);
for (Machine* machine : updatedMachines)
{
    machine->setRandomBreakdownMode(randomMode);
}

view.renderFactoryFloor(controllers, selectedMachineIndex);
view.renderInspector(controllers, selectedMachineIndex, tick, eventLogs);
view.renderEventLog(eventLogs);

view.renderStatistics(sim->getFinishedGoods(), sim->getTotalBreakdowns(), controllers);

        ImGui::Render();

        glViewport(0, 0, 1000, 700);
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        SDL_GL_SwapWindow(window);
    }

    delete sim;

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();

    SDL_GL_DeleteContext(gl_context);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
