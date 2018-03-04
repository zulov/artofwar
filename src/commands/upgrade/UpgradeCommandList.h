#pragma once
#include "commands/CommandList.h"
#include "simulation/SimulationObjectManager.h"

class UpgradeCommandList :
	public CommandList
{
public:
	explicit UpgradeCommandList(SimulationObjectManager* _simulationObjectManager);
	~UpgradeCommandList();

protected:
	void setParemeters(AbstractCommand* command) override;
private:
	SimulationObjectManager* simulationObjectManager;

};

