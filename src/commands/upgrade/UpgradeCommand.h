#pragma once
#include "commands/AbstractCommand.h"


enum class ActionType : char;
class SimulationObjectManager;

class UpgradeCommand : public AbstractCommand {
	friend class Stats;
public:
	UpgradeCommand(char player, int id, ActionType type);
	~UpgradeCommand() override;
	void execute() override;
	void setSimulationObjectManager(SimulationObjectManager* _simulationObjectManager);
private:
	ActionType type;
	short id;

	SimulationObjectManager* simulationObjectManager;
};
