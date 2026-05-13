#pragma once
#include "machine.h"
#include <string>

class MachineController {
private:
    Machine& m_machine; // 제어할 기계 모델의 참조 [cite: 102]

public:
    // 생성자: 제어할 기계 객체를 주입받습니다. [cite: 104]
    MachineController(Machine& machine) : m_machine(machine) {}

    // --- UI를 위한 데이터 제공 (Getter) ---
    
    // 현재 기계의 체력(HP)을 반환합니다.
    int getHp() const {
        return m_machine.gethp();
    }

    // 현재 기계의 대기열(Queue) 크기를 반환합니다.
    int getQueueSize() const {
        return m_machine.getQueueSize();
    }

    // UI에 표시할 상태 텍스트를 반환합니다. 
    // (모델의 상태가 바뀌더라도 UI 코드는 이 함수만 호출하면 됩니다) [cite: 111, 144]
    const char* getStatusLabel() const {
        // Machine 클래스에 status에 대한 public getter가 없으므로 
        // 직접 접근이 안 될 경우 모델에 getStatus() 추가를 권장합니다.
        // 여기서는 로직 예시를 위해 문자열을 반환하는 구조로 작성합니다.
        return "CHECK STATUS"; 
    }

    // --- UI의 입력을 모델로 전달 (Actions) ---

    // UI에서 '수리' 버튼을 눌렀을 때 호출될 함수입니다. [cite: 106, 139]
    void onRepairClicked() {
        m_machine.repair();
    }
};