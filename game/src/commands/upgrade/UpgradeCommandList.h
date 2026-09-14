#pragma once
#include <vector>

#include "scene/load/RuntimeSaveData.h"

class UpgradeCommand;
class SimulationObjectManager;

class UpgradeCommandList {
public:
	explicit UpgradeCommandList(SimulationObjectManager* simulationObjectManager);
	~UpgradeCommandList() = default;

	void add(UpgradeCommand* command);

	void execute();
	std::vector<PendingCommandSaveData> saveState(unsigned short& nextOrder) const;
private:
	std::vector<UpgradeCommand*> commands;
	SimulationObjectManager* simulationObjectManager;
};
