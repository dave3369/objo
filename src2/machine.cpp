#include "machine.h"
#include <iostream>
#include <cstdlib>

using namespace std;

// ==========================================
// 1. Machine 베이스 클래스 핵심 로직
// ==========================================
void Machine::update(int tick) {
    if (status == "Broken") return;

    completedThisTick = false;

    if (!currentItem) {
        currentItem = inputBuffer.pop();
        if (currentItem) {
            currentWorkTime = 0;
            status = "Working";
        } else {
            status = "Idle";
        }
    }

    if (currentItem && status == "Working") {
        process(); 
        decreasedhp();
        
        if (currentWorkTime >= processingTime) {
            completedThisTick = true;
            
            // 💡 다형성 호출이 아닌, 베이스 클래스의 공통 로직 실행 (RecipeManager 내부 작동)
            transformItem();
            finishProcess(); 
            
            status = "Idle";
            currentWorkTime = 0;
        }
    }

    if (randomBreakdownMode && status == "Working") {
        if (rand() % 1000 < breakdownChance()) {
            breakdown(); 
        }
    }
}


// ==========================================
// 2. 자식 기계 클래스들 구현부 (💡 제품 의존성 완벽히 제거됨)
// ==========================================

// --- Ultrafiltration (한외여과기) ---
void Ultrafilteration::process() {
    currentWorkTime++;
}

int Ultrafilteration::breakdownChance() const {
    return 5; 
}

// --- Dryer (건조기) ---
void Dryer::process() {
    currentWorkTime++;
}

int Dryer::breakdownChance() const {
    return 12; 
}

// --- Packaging (포장기) ---
void Packaging::process() {
    currentWorkTime++;
}

int Packaging::breakdownChance() const {
    return 3; 
}


// ==========================================
// 3. FactorySimulation 매니저 구현부
// ==========================================
void FactorySimulation::start(std::vector<std::unique_ptr<Machine>> builtPipeline) {
    machines = std::move(builtPipeline);
    
    itemCounter = 0;
    finishedGoods = 0;
    totalBreakdowns = 0;
}

void FactorySimulation::runTick() {
    // 기존 가공 유효성 체크 및 틱 루프 로직 유지
    int currentWip = 0; 
    for (const auto& m : machines) {
        currentWip += m->getQueueSize();
        if (m->hasCurrentItem()) currentWip++;
    }

    if (currentWip < 5) {
        // 첫 번째 공정에 새 원유 투입 (주입된 파이프라인 구성에 맞춰 유연하게 대입)
        if (!machines.empty()) {
            machines[0]->receiveItem(make_unique<RawMilk>(itemCounter++));
        }
    }

    int currentBreakdowns = 0;
    for (const auto& m : machines) {
        if (m->getStatus() == "Broken") currentBreakdowns++;
    }
    totalBreakdowns = currentBreakdowns; 

    static int currentTick = 0;
    currentTick++;

    for (const auto& m : machines) {
        m->update(currentTick);
    }
    
    if (!machines.empty() && machines.back()->didCompleteThisTick()) {
        finishedGoods++;
    }
}