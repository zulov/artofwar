#pragma once
#include "CommandList.h"
#include "simulation/SimulationObjectManager.h"
#include "CreationCommand.h"

class SimulationObjectManager;

class SimulationCommandList :public CommandList
{
public:
	SimulationCommandList(SimulationObjectManager * _simulationObjectManager);
	virtual ~SimulationCommandList();
protected:
	void setParemeters(AbstractCommand* command) override;
private:
	SimulationObjectManager * simulationObjectManager;
};

