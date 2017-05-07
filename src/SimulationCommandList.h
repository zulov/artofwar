#pragma once
#include "CommandList.h"
#include "SpacingType.h"
#include "BuildingType.h"
#include "SimulationObjectManager.h"
#include "SimulationCommand.h"

class SimulationObjectManager;

class SimulationCommandList :public CommandList
{
public:
	SimulationCommandList(SimulationObjectManager * _simulationObjectManager);
	~SimulationCommandList();
protected:
	virtual void setParemeters(AbstractCommand* command) override;
private:
	SimulationObjectManager * simulationObjectManager;
};

