#include "machine.h"
#include <iostream>
#include <algorithm>
#include <random>
#include <cstdlib>
#include <ctime>
using namespace std;

ItemBuffer::~ItemBuffer(){
    while(!q.empty()){
        delete q.front();
        q.pop();
    }
}
void ItemBuffer::push(Item* item) { q.push(item); }
Item* ItemBuffer::pop() {
    if(q.empty()) return nullptr;
    Item* item = q.front();
    q.pop();
    return item;
}
Item* ItemBuffer::front() const { return q.empty() ? nullptr : q.front(); }
bool ItemBuffer::isEmpty() const { return q.empty(); }
int ItemBuffer::getSize() const { return q.size(); }

Machine::Machine(int time, string initStatus) : processingTime(time), status(initStatus),currentWorkTime(0),hp(100) {}

Machine::~Machine() {
    if(currentItem) delete currentItem;
}

void Machine::setNextMachine(Machine *next) { 
    nextMachine = next; 
}

void Machine::receiveItem(Item* item) { 
    inputBuffer.push(item);

}
int Machine::getQueueSize() const {
    return inputBuffer.getSize();
}


void Machine::repair(){
    hp = 100; 
    status = (currentItem != nullptr) ? "WORKING" : "IDLE";}

int Machine::gethp() const{return hp;}

void Machine::decreasedhp(){
    if (rand() % 100 < breakdownChance()) {
        int damage = (rand() % 2) + 1;
        hp -= damage;
    }
}
void Machine::breakdown(){
    if(hp <= 0){
        hp = 0;
        status = "BROKEN";
    }
}
void Machine::update() {
    if (status == "BROKEN") return;

    // 1. 공통 고장 판정
    decreasedhp();
    breakdown();
    if (status == "BROKEN") return;

    // 2. 작업 시작 및 대기 로직
    if (currentItem == nullptr) {
        if (!inputBuffer.isEmpty()) {
            currentItem = inputBuffer.pop();
            currentWorkTime = 0; 
            status = "WORKING";
        } else {
            status = "IDLE";
            return;
        }
    }

    // 3. 현재 작업 중이면 자식 클래스의 구체적인 process 실행
    if (status == "WORKING") {
        process(); // 다형성 호출

        // 작업이 완료되었는지 확인
        if (currentWorkTime >= processingTime) {
            if (nextMachine != nullptr) {
                nextMachine->receiveItem(currentItem); // 다음 기계로 전달
            } else {
                // 파이프라인의 끝 (완제품 처리 로직 추가 가능)
                delete currentItem; 
            }
            currentItem = nullptr;
            currentWorkTime = 0;
            status = "IDLE";
        }
    }
}

void Ultrafilteration::process() {
    currentWorkTime++;
    // 필터링 특화 부가 로직이 있다면 여기에 추가
}
int Ultrafilteration::breakdownChance() const {
    return 10;
}

void Dryer::process() {
    currentWorkTime++;
    // 건조 특화 부가 로직이 있다면 여기에 추가
}
int Dryer::breakdownChance() const {
    return 20;
}

void Packaging::process() {
    currentWorkTime++;
    // 포장 특화 부가 로직이 있다면 여기에 추가
}
int Packaging::breakdownChance() const {
    return 0;
}
// === 팩토리 시뮬레이션 설정 ===

FactorySimulation::~FactorySimulation() {
    for (Machine* m : machines) {
        delete m;
    }
}

void FactorySimulation::start() {
    // 공정 시간 설정: 필터(3), 건조기(7 - 더 오래걸림), 포장(2)
    Machine* filter = new Ultrafilteration(3, "Idle");
    Machine* dryer = new Dryer(7, "Idle");
    Machine* packager = new Packaging(2, "Idle");

    filter->setNextMachine(dryer);
    dryer->setNextMachine(packager);

    machines.push_back(filter);
    machines.push_back(dryer);
    machines.push_back(packager);

    // 초기 테스트용 원료(Item) 5개 투입
    for(int i=0; i<5; ++i) {
        filter->receiveItem(new Item{i});
    }
}

void FactorySimulation::runTick() {
    for (Machine* m : machines) {
        m->update();
    }
}






