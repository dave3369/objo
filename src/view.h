#pragma once
#include "machineController.h"
#include <vector>
#include <string>
using namespace std;


class SimulationView {
private:
    // ⭐ 핵심 포인트 2: UI 전용 상태 변수의 캡슐화
    // 기존에 main.cpp를 지저분하게 만들던 'UI의 상태를 관리하는 변수'들을 View 내부로 가져옵니다.
    std::vector<MachineController> m_machineControllers;
    int selectedMachineIndex;   // 현재 마우스로 클릭해 선택한 기계 번호
    int selectedScenario;       // 선택된 시나리오 (0: 일반, 1: 랜덤 고장)
    int speed;                  // 시뮬레이션 속도 슬라이더 값 (1~5)
    bool simulationRunning;     // 시뮬레이션이 실행 중인지 여부 (Start/Pause)
    bool resetRequested;        // 유저가 리셋 버튼을 눌렀는지 여부
    int lastFinishedGoods;      // 이전 프레임의 완제품 개수 (새 제품 체킹용)
    
    // 화면의 'Event Log' 창에만 그려질 로그 데이터 배열
    std::deque<std::string> eventLogs; 

public:
    // 생성자에서 UI 변수들의 초기값을 세팅합니다.
    SimulationView();
    ~SimulationView() = default;

    // ⭐ 핵심 포인트 3: 오직 Controller만 받는 렌더링 인터페이스
    // 매 프레임마다 메인 루프에서 호출되며, ImGui 화면(Control, Floor, Inspector 등)을 그립니다.
    // 데이터를 모델에서 직접 꺼내지 않고, factoryCtrl의 Getter 함수들을 거쳐서 안전하게 읽어옵니다.
    void render(FactoryController& factoryCtrl, int tick);

    // ⭐ 핵심 포인트 4: main.cpp(메인 시스템)와 UI 간의 소통 창구 (Getter / Setter)
    // main.cpp는 이 함수들을 통해 "지금 UI에서 스타트 버튼이 눌렸나?", "배속이 몇인가?"를 확인하고 시뮬레이션을 업데이트합니다.
    bool isSimulationRunning() const { return simulationRunning; }
    void setSimulationRunning(bool running) { simulationRunning = running; }
    
    int getSpeed() const { return speed; }
    int getSelectedScenario() const { return selectedScenario; }
    
    bool isResetRequested() const { return resetRequested; }
    void clearResetRequest() { resetRequested = false; }
    
    std::deque<std::string>& getEventLogs() { return eventLogs; }
    int& getLastFinishedGoods() { return lastFinishedGoods; }
};