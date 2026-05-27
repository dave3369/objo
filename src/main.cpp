#include "imgui.h"
#include "imgui_impl_sdl2.h"
#include "imgui_impl_opengl3.h"

#include <SDL.h>
#include <SDL_opengl.h>
#include <deque>

#include "machine.h"
#include "machineController.h"
#include "view.h"
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

    // 2. 컨트롤러 및 뷰 생성
    // (리셋 시 재할당을 쉽게 하기 위해 컨트롤러도 포인터로 생성합니다)
    FactoryController* factoryCtrl = new FactoryController(*sim);
    SimulationView view;

    // main에 남아있어야 할 변수는 시간 측정과 틱 카운트뿐입니다.
    int tick = 0;
    double lastTickTime = ImGui::GetTime();

    while (running)
    {
        // =========================
        // 1. SDL 이벤트 처리 (사용자 입력 및 창 닫기)
        // =========================
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            ImGui_ImplSDL2_ProcessEvent(&event);
            if (event.type == SDL_QUIT)
                running = false;
        }

        // =========================
        // 2. ImGui 새 프레임 시작
        // =========================
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplSDL2_NewFrame();
        ImGui::NewFrame();

        // =========================
        // 3. 리셋(Reset) 처리
        // View에서 리셋 버튼이 눌렸다면 모델과 컨트롤러를 초기화합니다.
        // =========================
        if (view.isResetRequested())
        {
            delete factoryCtrl;
            delete sim;

            sim = new FactorySimulation();
            sim->start();
            factoryCtrl = new FactoryController(*sim);

            tick = 0;
            lastTickTime = ImGui::GetTime();
            
            view.setSimulationRunning(false);
            view.getLastFinishedGoods() = 0;
            view.getEventLogs().push_front("[Tick 0] Simulation reset");
            view.clearResetRequest();
        }

        // =========================
        // 4. 모델(Simulation) 업데이트
        // =========================
        double currentTime = ImGui::GetTime();
        double interval = 1.0 / view.getSpeed();

        // View에서 선택한 시나리오(랜덤 고장 모드)를 모델에 반영
        bool randomMode = (view.getSelectedScenario() == 1);
        for (Machine* machine : sim->getMachines())
        {
            machine->setRandomBreakdownMode(randomMode);
        }

        // 시뮬레이션이 실행 중이고, 지정된 속도(interval)만큼 시간이 지났다면 Tick 실행
        if (view.isSimulationRunning() && (currentTime - lastTickTime >= interval))
        {
            sim->runTick();
            tick++;

            // 완제품 생산 감지 및 로그 추가
            if (sim->getFinishedGoods() > view.getLastFinishedGoods())
            {
                view.getEventLogs().push_front(
                    "[Tick " + std::to_string(tick) + "] Finished goods completed"
                );
                view.getLastFinishedGoods() = sim->getFinishedGoods();
            }

            lastTickTime = currentTime;
        }

        // =========================
        // 5. UI 렌더링 (View 호출)
        // 화면을 그리는 복잡한 로직은 모두 View 객체에 위임합니다.
        // =========================
        view.render(*factoryCtrl, tick);

        // =========================
        // 6. OpenGL 화면 출력 (버퍼 스왑)
        // =========================
        ImGui::Render();
        glViewport(0, 0, (int)ImGui::GetIO().DisplaySize.x, (int)ImGui::GetIO().DisplaySize.y);
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
