#pragma once
#include <string>
#include <vector>

// 1. 관찰자 인터페이스 (알림 수신용)
class IObserver {
public:
    virtual ~IObserver() = default;
    virtual void onNotify(const std::string& machineName, const std::string& eventType) = 0;
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
