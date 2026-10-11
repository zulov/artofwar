#pragma once
#include "commands/PhysicalCommand.h"

enum class ActionType : unsigned char;
enum class QueueActionType : unsigned char;
class SimulationObjectManager;
struct PendingCommandSaveData;

class UpgradeCommand {
	friend class Stats;

public:
	UpgradeCommand(unsigned char playerId, unsigned short id, QueueActionType type, unsigned short levelId);
	~UpgradeCommand() = default;

	void execute(const SimulationObjectManager* simulationObjectManager) const;
	PendingCommandSaveData saveState(unsigned short order) const;

private:
	QueueActionType type;
	unsigned char playerId;
	unsigned short id;
	unsigned short levelId;
};
