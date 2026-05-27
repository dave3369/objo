#pragma once
#include "machine.h"
#include <vector>
#include <string>

class MachineController {
private:
    Machine& m_machine;
public:
    MachineController(Machine& machine) : m_machine(machine) {}

    std::string getName() const { return m_machine.getName(); }
    std::string getStatus() const { return m_machine.getStatus(); }
    int getHp() const { return m_machine.gethp(); }
    int getQueueSize() const { return m_machine.getQueueSize(); }
    int getProgress() const { return m_machine.getProgress(); }
    int getProcessingTime() const { return m_machine.getProcessingTime(); }
    
    void onRepairClicked() { m_machine.repair(); }
    void onForceBreakClicked() { m_machine.forceBreak(); }

};

class FactoryController {
private:
    FactorySimulation& m_sim;
    std::vector<MachineController> m_machineControllers;

public:
    FactoryController(FactorySimulation& sim) : m_sim(sim) {
       
        for (Machine* machine : m_sim.getMachines()) {
            m_machineControllers.push_back(MachineController(*machine));
        }
    }

    int getFinishedGoods() const { return m_sim.getFinishedGoods(); }
    int getTotalBreakdowns() const { return m_sim.getTotalBreakdowns(); }
    
   
    std::vector<MachineController>& getMachineControllers() { 
        return m_machineControllers; 
    }

    void remapControllers() {
        m_machineControllers.clear();
        for (Machine* machine : m_sim.getMachines()) {
            m_machineControllers.push_back(MachineController(*machine));
        }
    }
    int getLastFinishedGoods() const { return m_sim.getFinishedGoods(); }
   
};