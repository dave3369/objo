#pragma once
#include "machine.h"
#include <string>

class MachineController {
private:
    Machine& machine;

public:
    MachineController(Machine& m) : machine(m) {}

    std::string getName() const {
        return machine.getName();
    }

    std::string getStatus() const {
        return machine.getStatus();
    }

    int getHp() const {
        return machine.gethp();
    }

    int getQueueSize() const {
        return machine.getQueueSize();
    }

    int getProgress() const {
        return machine.getProgress();
    }

    int getProcessingTime() const {
        return machine.getProcessingTime();
    }

    bool hasCurrentItem() const {
        return machine.hasCurrentItem();
    }

    void repair() {
        machine.repair();
    }

    void forceBreak() {
        machine.forceBreak();
    }
};
