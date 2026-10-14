#pragma once
#include "commands/PhysicalCommand.h"

enum class ActionType : unsigned char;
enum class UpgradeType : char;
class SimulationObjectManager;
struct PendingCommandSaveData;

class UpgradeCommand {
	friend class Stats;

public:
	UpgradeCommand(unsigned char playerId, unsigned short id, UpgradeType type);
	~UpgradeCommand() = default;

	void execute(const SimulationObjectManager* simulationObjectManager) const;
	PendingCommandSaveData saveState(unsigned short order) const;

private:
	UpgradeType type;
	unsigned char playerId;
	unsigned short id;
};
