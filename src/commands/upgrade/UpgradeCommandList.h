#pragma once
#include "commands/CommandList.h"

class SimulationObjectManager;

class UpgradeCommandList :
	public CommandList
{
public:
	explicit UpgradeCommandList(SimulationObjectManager* simulationObjectManager);
	~UpgradeCommandList() = default;

protected:
	void setParameters(AbstractCommand* command) override;
private:
	SimulationObjectManager* simulationObjectManager;
};
