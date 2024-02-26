#pragma once
#include <vector>

class UpgradeCommand;
class SimulationObjectManager;

class UpgradeCommandList {
public:
	explicit UpgradeCommandList(SimulationObjectManager* simulationObjectManager);
	~UpgradeCommandList() = default;

	void add(UpgradeCommand* command);

	void execute();
private:
	std::vector<UpgradeCommand*> commands;
	SimulationObjectManager* simulationObjectManager;
};
