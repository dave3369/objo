#include "imgui.h"
#include "imgui_impl_sdl2.h"
#include "imgui_impl_opengl3.h"

#include <SDL.h>
#include <SDL_opengl.h>
#include <deque>

#include "machine.h"

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

    bool simulationRunning = false;
    int tick = 0;
    int speed = 1;
    int lastFinishedGoods = 0;
    int selectedMachineIndex = 0;

    int selectedScenario = 0;
    const char* scenarios[] = { "Normal Flow", "Bottleneck", "Random Breakdowns", "Overflow" };
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

        ImGui::Begin("Factory Simulation Control");

        ImGui::Text("Current Tick: %d", tick);

        if (ImGui::Button("Start"))
        {
            simulationRunning = true;
        }

        ImGui::SameLine();

        if (ImGui::Button("Pause"))
        {
            simulationRunning = false;
        }

        ImGui::SameLine();

        if (ImGui::Button("Reset"))
	{
    	    resetRequested = true;
	}

        ImGui::SliderInt("Speed", &speed, 1, 5);

	ImGui::Combo("Scenario",
             &selectedScenario,
             scenarios,
             IM_ARRAYSIZE(scenarios));

    for (Machine* machine : sim->getMachines())
    {
        if (selectedScenario == 2) { 
            machine->setRandomBreakdownMode(true);
        } else {
            machine->setRandomBreakdownMode(false);
        }
        
    }

    ImGui::End();

    // 2. 리셋 버튼 눌렀을 때 로직
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
            machine->setRandomBreakdownMode(selectedScenario == 2);
        }

        eventLogs.push_front("[Tick 0] Simulation reset");
        resetRequested = false;
    }
	ImGui::End();

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

	const auto& machines = sim->getMachines();

        ImGui::Begin("Factory Floor");

        ImGui::Text("Machines (click to inspect)");
	ImGui::Separator();

	for (int i = 0; i < machines.size(); i++)
	{
    	Machine* machine = machines[i];

    	bool selected = (selectedMachineIndex == i);

    	if (ImGui::Selectable(machine->getName().c_str(), selected))
    	{
        	selectedMachineIndex = i;
    	}

	    ImGui::SameLine();

	    if (machine->getStatus() == "WORKING")
    	{
        	ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "WORKING");
    	}
    	else if (machine->getStatus() == "BROKEN")
    	{
        	ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "BROKEN");
    	}
	    else
    	{
        	ImGui::Text("%s", machine->getStatus().c_str());
    	}
	}

	ImGui::End();

	ImGui::Begin("Inspector");

	if (!machines.empty())
	{
	    Machine* selectedMachine = machines[selectedMachineIndex];

	    ImGui::Text("Selected Machine: %s", selectedMachine->getName().c_str());
    	    ImGui::Separator();

    	    if (selectedMachine->getStatus() == "WORKING")
    	    {
        	ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f),
                	           "Status: %s",
                        	   selectedMachine->getStatus().c_str());
    	    }
    	    else if (selectedMachine->getStatus() == "BROKEN")
    	    {
        	ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f),
                	           "Status: %s",
                        	   selectedMachine->getStatus().c_str());
    	    }
    	    else
    	    {
        	ImGui::Text("Status: %s", selectedMachine->getStatus().c_str());
    	    }

    	    ImGui::Text("HP: %d", selectedMachine->gethp());
    	    ImGui::ProgressBar(selectedMachine->gethp() / 100.0f, ImVec2(220, 20));

    	    float workProgress = 0.0f;
    	    if (selectedMachine->getProcessingTime() > 0)
    	    {
        	workProgress = (float)selectedMachine->getProgress() /
                	       selectedMachine->getProcessingTime();
    	    }

    	    ImGui::Text("Progress: %d / %d",
            	        selectedMachine->getProgress(),
                	selectedMachine->getProcessingTime());

	    ImGui::ProgressBar(workProgress, ImVec2(220, 20));

	    ImGui::Text("Queue Size: %d", selectedMachine->getQueueSize());
    	    ImGui::Text("Process Time: %d", selectedMachine->getProcessingTime());

	    ImGui::Separator();

    	    if (ImGui::Button("Force Break"))
	    {
    		selectedMachine->forceBreak();

    		eventLogs.push_front(
        	    "[Tick " + std::to_string(tick) + "] "
        	    + selectedMachine->getName() + " was force broken"
    		);
	    }

    	    ImGui::SameLine();

	    if (ImGui::Button("Instant Repair"))
	    {
    		selectedMachine->repair();

	        eventLogs.push_front(
        	    "[Tick " + std::to_string(tick) + "] "
        	    + selectedMachine->getName() + " was repaired"
    		);
	    }
	}

	ImGui::End();

	ImGui::Begin("Event Log");

	if (ImGui::Button("Clear Log"))
	{
    	    eventLogs.clear();
	}

	ImGui::Separator();

	ImGui::BeginChild("LogScroll", ImVec2(0, 150), true);

	for (const std::string& log : eventLogs)
	{
	    ImGui::Text("%s", log.c_str());
	}

	ImGui::EndChild();

	ImGui::End();

	ImGui::Begin("Statistics");

	int wipCount = 0;

	for (Machine* machine : machines)
	{
    	    wipCount += machine->getQueueSize();

    	    if (machine->hasCurrentItem())
    	    {
        	wipCount++;
    	    }
	}

	ImGui::Text("Finished Goods: %d", sim->getFinishedGoods());
	ImGui::Text("WIP Count: %d", wipCount);
	ImGui::Text("Total Breakdowns: %d", sim->getTotalBreakdowns());
	ImGui::Text("Lost Products: %d", 0);

	ImGui::End();

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
