#pragma once
#include <string>
#include <queue>
#include <vector>
using namespace std;

struct Item{
    int id;
};

class ItemBuffer{
    private:
    queue<Item*> q;
    public:
    ~ItemBuffer();
    void push(Item *item);
    Item *pop();
    Item *front()const;
    bool isEmpty() const;
    int getSize() const;
};

class Machine{

    protected:
    string status;
    int hp;
    int processingTime;
    int currentWorkTime;
    string name;

    Item* currentItem = nullptr;
    Machine* nextMachine = nullptr;

    bool randomBreakdownMode = false;
    bool completedThisTick = false;

    private:
    ItemBuffer inputBuffer;

    public:
    Machine(int time, string initStatus, string machineName);
    virtual ~Machine();

    void setNextMachine(Machine *next);
    void receiveItem(Item* item);
    int getQueueSize() const;
    string getStatus() const;
    int getProgress() const;
    int getProcessingTime() const;
    bool hasCurrentItem() const;
    bool didCompleteThisTick() const;
    string getName() const;

    void repair();
    void forceBreak();
    void setRandomBreakdownMode(bool enabled);
    int gethp() const;
    void decreasedhp();
    void breakdown();
    
    // 다형성을 위한 가상 함수들
    void update(); // 템플릿 메서드 (뼈대 역할)
    virtual void process() = 0; // 자식 클래스가 구현할 실제 공정
    virtual int breakdownChance() const = 0; // 자식 클래스별 고장 확률
};

class Ultrafilteration :public Machine{
    public:
    Ultrafilteration(int time, string initStatus) : Machine(time, initStatus, "Ultrafiltration") {}
    void process() override ;
    int breakdownChance() const override;
   
};

class Dryer :public Machine{
    public:
    Dryer(int time, string initStatus) : Machine(time, initStatus, "Dryer") {}
    void process() override ;
    int breakdownChance() const override;
    
};

class Packaging :public Machine{
    public:
    Packaging(int time, string initStatus) : Machine(time, initStatus, "Packaging") {}
    void process() override ;
    int breakdownChance() const override;
  
};

class FactorySimulation{
    private:
    vector <Machine*> machines;
    int finishedGoods = 0;
    int totalBreakdowns = 0;
    public:
    ~FactorySimulation();
    void start();
    void runTick();
    const vector<Machine*>& getMachines() const { return machines; };
    int getFinishedGoods() const { return finishedGoods; }
    int getTotalBreakdowns() const { return totalBreakdowns; }
};
