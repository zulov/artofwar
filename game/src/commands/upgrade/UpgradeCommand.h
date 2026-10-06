#pragma once
#include "commands/PhysicalCommand.h"

enum class ActionType : unsigned char;
enum class QueueActionType : unsigned char;
class SimulationObjectManager;
struct PendingCommandSaveData;

class UpgradeCommand {
	friend class Stats;

public:
	UpgradeCommand(unsigned char playerId, short id, QueueActionType type, short levelId = -1);
	~UpgradeCommand() = default;

	void execute(SimulationObjectManager* simulationObjectManager) const;
	void setSimulationObjectManager(SimulationObjectManager* _simulationObjectManager);
	PendingCommandSaveData saveState(unsigned short order) const;

private:
	QueueActionType type;
	unsigned char playerId;
	short id;
	short levelId;

	SimulationObjectManager* simulationObjectManager;
};
