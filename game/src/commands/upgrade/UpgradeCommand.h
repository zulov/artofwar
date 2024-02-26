#pragma once
#include "commands/PhysicalCommand.h"

enum class ActionType : char;
enum class QueueActionType : char;
class SimulationObjectManager;

class UpgradeCommand {
	friend class Stats;
public:
	UpgradeCommand(char player, short id, QueueActionType type);
	~UpgradeCommand() = default;

	void execute(SimulationObjectManager* simulationObjectManager) const;
	void setSimulationObjectManager(SimulationObjectManager* _simulationObjectManager);
private:
	QueueActionType type;
	short id;
	char player;

	SimulationObjectManager* simulationObjectManager;
};
