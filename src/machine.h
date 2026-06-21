#pragma once
#include <string>
#include <queue>
#include <vector>
#include <memory>
#include <map>
#include <functional>
#include "SimulationCore.h"
#include "SimulationBridge.h"


class RecipeManager {
public:
    using TransformerFunc = std::function<std::unique_ptr<Product>(int)>;

    static RecipeManager& getInstance() {
        static RecipeManager instance;
        return instance;
    }

    // 외부에서 새로운 변환 규칙(레시피)을 동적으로 등록 가능 (확장성)
    void registerRecipe(const std::string& machineName, const std::string& inputName, TransformerFunc transformer) {
        registry[machineName][inputName] = transformer;
    }

    // 기계 이름과 투입 아이템을 기반으로 변환 수행
    std::unique_ptr<Product> transform(const std::string& machineName, std::unique_ptr<Product> item) {
        if (!item) return nullptr;
        
        std::string inputName = item->getName();
        int id = item->getId();

        // 등록된 레시피가 있다면 변환 실행
        if (registry.count(machineName) && registry[machineName].count(inputName)) {
            return registry[machineName][inputName](id);
        }
        
        // 레시피가 없으면 변환 없이 그대로 반환 (예: Packaging)
        return item; 
    }

private:
    std::map<std::string, std::map<std::string, TransformerFunc>> registry;
    
    // 생성자에서 초기 팩토리 라인의 기본 레시피를 등록
    RecipeManager() {
        registerRecipe("Ultrafiltration", "Raw Milk", [](int id) { 
            return std::make_unique<LiquidWhey>(id); 
        });
        registerRecipe("Dryer", "Liquid Whey", [](int id) { 
            return std::make_unique<WPCPowder>(id); 
        });
    }
};

// 1. 스마트 포인터를 적용한 버퍼
class ItemBuffer {
private:
    std::queue<std::unique_ptr<Product>> q;
public:
    
    ~ItemBuffer() {
        clear();
    }
    void push(std::unique_ptr<Product> item) { q.push(std::move(item)); }
    std::unique_ptr<Product> pop() {
        if (q.empty()) return nullptr;
        auto item = std::move(q.front());
        q.pop();
        return item;
    }
    void clear() {
        while (!q.empty()) {
            q.pop(); 
        }
    }
    std::unique_ptr<Product>& front() { return q.front(); }
    bool isEmpty() const { return q.empty(); }
    int getSize() const { return q.size(); }
};

// 2. Machine 베이스 클래스
class Machine : public SimulationObject {
protected:
    std::string status;
    int hp;
    int processingTime;
    int currentWorkTime = 0;
    std::string name;

    std::unique_ptr<Product> currentItem;
    Machine* nextMachine = nullptr;

    bool randomBreakdownMode = false;
    bool completedThisTick = false;

    ItemBuffer inputBuffer;
    int outputCount = 0;
    int lostProductsCount = 0;

public:
    Machine(int time, std::string initStatus, std::string machineName)
        : processingTime(time), status(initStatus), name(machineName), hp(100) {}
    virtual ~Machine() = default;
    std::string getCurrentItemName() const {
        if (currentItem) return currentItem->getName();
        return "None";
    }

    MachineSnapshot getSnapshot() const {
    MachineSnapshot snap;
    snap.name = name;
    snap.status = status;
    snap.hp = hp;
    snap.queueSize = getQueueSize();
    snap.progress = currentWorkTime;
    snap.processingTime = processingTime;
    snap.hasCurrentItem = (currentItem != nullptr);
    snap.currentItemName = getCurrentItemName();
    snap.outputCount = outputCount;
    return snap;
    }

    std::string getInfo() const override { return name; }
    std::string getName() const { return name; }
    std::string getStatus() const { return status; }
    int gethp() const { return hp; }
    int getQueueSize() const { return inputBuffer.getSize(); }
    int getProgress() const { return currentWorkTime; }
    int getProcessingTime() const { return processingTime; }
    bool hasCurrentItem() const { return currentItem != nullptr; }
    bool didCompleteThisTick() const { return completedThisTick; }
    
    void setRandomBreakdownMode(bool enabled) { randomBreakdownMode = enabled; }
    void repair() { hp = 100; 
        if (currentItem) {
        status = "WORKING";
    } else {
        status = "IDLE";
    } }
    void forceBreak() { hp = 0; breakdown(); }
    void decreasedhp() { hp -= 1; if(hp <= 0) breakdown(); }
    
    void setNextMachine(Machine* next) { nextMachine = next; }
    void receiveItem(std::unique_ptr<Product> item) { inputBuffer.push(std::move(item)); }
    int getLostProductsCount() const { return lostProductsCount; }
    void breakdown() {
        hp -= 1;
        status = "BROKEN";
        if (currentItem) {
            currentItem.reset(); // 아이템을 파괴(메모리 해제)하여 소실 처리
            lostProductsCount++; // 유실 카운트 증가
            currentWorkTime = 0; // 작업 진행도 초기화
            notifyObservers("BREAKDOWN_ITEM_LOST"); // 전용 이벤트 알림
        } else {
            notifyObservers("BREAKDOWN");
        }
    }
    void finishProcess() {
        outputCount++;
        notifyObservers("PROCESS_COMPLETE"); 
        if (nextMachine && currentItem) {
            nextMachine->receiveItem(std::move(currentItem)); 
        } else if (!nextMachine && currentItem) {
            currentItem.reset(); 
        }
    }

    void update(int tick) override;
    virtual void process() = 0;
    virtual int breakdownChance() const = 0;

    // 💡 변경됨: 자식 클래스들의 중복 오버라이딩을 제거하고 베이스 클래스가 RecipeManager에 위임
    void transformItem() {
        currentItem = RecipeManager::getInstance().transform(name, std::move(currentItem));
    }
};

// 3. 자식 클래스들 복구 (💡 transformItem 관련 코드가 완전히 제거되어 극도로 슬림해짐)
class Ultrafilteration : public Machine {
public:
    Ultrafilteration(int time, std::string initStatus) : Machine(time, initStatus, "Ultrafiltration") {}
    void process() override;
    int breakdownChance() const override;
};

class Dryer : public Machine {
public:
    Dryer(int time, std::string initStatus) : Machine(time, initStatus, "Dryer") {}
    void process() override;
    int breakdownChance() const override;
};

class Packaging : public Machine {
public:
    Packaging(int time, std::string initStatus) : Machine(time, initStatus, "Packaging") {}
    void process() override;
    int breakdownChance() const override;
};

class FactoryBuilder {
private:
    std::vector<std::unique_ptr<Machine>> pipeline;

public:
    FactoryBuilder() = default;

    // Fluent Interface (메서드 체이닝) 제공
    FactoryBuilder& addUltrafiltration(int time, std::string initStatus = "IDLE") {
        pipeline.push_back(std::make_unique<Ultrafilteration>(time, initStatus));
        return *this;
    }

    FactoryBuilder& addDryer(int time, std::string initStatus = "IDLE") {
        pipeline.push_back(std::make_unique<Dryer>(time, initStatus));
        return *this;
    }

    FactoryBuilder& addPackaging(int time, std::string initStatus = "IDLE") {
        pipeline.push_back(std::make_unique<Packaging>(time, initStatus));
        return *this;
    }

    // 최종적으로 기계 간의 전/후관계를 선형으로 자동 매핑하고 파이프라인 반환
    std::vector<std::unique_ptr<Machine>> buildPipeline() {
        if (!pipeline.empty()) {
            for (size_t i = 0; i < pipeline.size() - 1; ++i) {
                pipeline[i]->setNextMachine(pipeline[i + 1].get());
            }
            // 마지막 기계는 자동으로 nextMachine이 nullptr이 됨
        }
        return std::move(pipeline);
    }
};

// 3. 시뮬레이션 매니저
class FactorySimulation {
private:
    std::vector<std::unique_ptr<Machine>> machines;
    int finishedGoods = 0;
    int totalBreakdowns = 0;
    int itemCounter = 0;
public:
    ~FactorySimulation() = default;
    
    // 💡 변경됨: 하드코딩을 없애고 외부에서 빌드된 파이프라인을 주입받아 시작
    void start(std::vector<std::unique_ptr<Machine>> builtPipeline);
    void runTick();
    
    const std::vector<std::unique_ptr<Machine>>& getMachines() const { return machines; }
    int getFinishedGoods() const { return finishedGoods; }
    int getTotalBreakdowns() const { return totalBreakdowns; }
    int getTotalLostProducts() const {
        int total = 0;
        for (const auto& m : machines) {
            total += m->getLostProductsCount();
        }
        return total;
    }
};
