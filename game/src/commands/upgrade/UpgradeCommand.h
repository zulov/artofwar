#pragma once
#include "commands/PhysicalCommand.h"

enum class ActionType : unsigned char;
enum class QueueActionType : unsigned char;
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
	char player;
	short id;

	SimulationObjectManager* simulationObjectManager;
};
