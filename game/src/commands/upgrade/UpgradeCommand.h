#pragma once
#include "commands/AbstractCommand.h"

enum class ActionType : char;
enum class QueueActionType : char;
class SimulationObjectManager;

class UpgradeCommand : public AbstractCommand {
	friend class Stats;
public:
	UpgradeCommand(char player, int id, QueueActionType type);
	~UpgradeCommand() override = default;
	void execute() override;
	void setSimulationObjectManager(SimulationObjectManager* _simulationObjectManager);
private:
	QueueActionType type;
	short id;

	SimulationObjectManager* simulationObjectManager;
};