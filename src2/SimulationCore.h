#pragma once
#include <string>
#include <vector>

// 1. 관찰자 인터페이스 (알림 수신용)
class IObserver {
public:
    virtual ~IObserver() = default;
    virtual void onNotify(const std::string& machineName, const std::string& eventType) = 0;
};
class SimulationLogger : public IObserver {
private:
    std::deque<std::string> eventLogs;
    int currentTick = 0;

public:
    // 틱 동기화
    void setTick(int tick) { currentTick = tick; }
    
    // 자동 이벤트 수신 (Observer 패턴)
    void onNotify(const std::string& machineName, const std::string& eventType) override {
        std::string logMsg = "[Tick " + std::to_string(currentTick) + "] " + machineName;
        if (eventType == "BREAKDOWN") logMsg += " has broken down!";
        else if (eventType == "BREAKDOWN_ITEM_LOST") { 
            logMsg += " broke down and LOST its current item!"; 
        }
        else if (eventType == "PROCESS_COMPLETE") logMsg += " finished its process.";
        else logMsg += " triggered event: " + eventType;
        
        eventLogs.push_front(logMsg);
    }

    // UI(수동 조작) 등 외부에서 로그를 직접 남길 때 사용
    void addManualLog(const std::string& message) {
        eventLogs.push_front("[Tick " + std::to_string(currentTick) + "] " + message);
    }

    // Getter 및 상태 제어
    const std::deque<std::string>& getLogs() const { return eventLogs; }
    void clearLog() { eventLogs.clear(); }
};

class SimulationObject {
protected:
    std::vector<IObserver*> observers;
    void notifyObservers(const std::string& eventType) {
        for (auto* obs : observers) {
            obs->onNotify(getInfo(), eventType);
        }
    }
public:
    virtual ~SimulationObject() = default;
    virtual void update(int tick) = 0; // 다형성 루프 핵심
    virtual std::string getInfo() const = 0;
    
    void addObserver(IObserver* obs) { observers.push_back(obs); }
};

// 2. 캡슐화된 WPC 제품 족보
class Product {
private:
    int id;
public:
    Product(int id) : id(id) {}
    virtual ~Product() = default;
    int getId() const { return id; }
    virtual std::string getName() const = 0;
};

class RawMilk : public Product {
public:
    RawMilk(int id) : Product(id) {}
    std::string getName() const override { return "Raw Milk"; }
};

class LiquidWhey : public Product {
public:
    LiquidWhey(int id) : Product(id) {}
    std::string getName() const override { return "Liquid Whey"; }
};

class WPCPowder : public Product {
public:
    WPCPowder(int id) : Product(id) {}
    std::string getName() const override { return "WPC Powder"; }
};
