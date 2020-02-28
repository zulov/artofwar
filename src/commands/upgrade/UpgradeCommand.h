#pragma once
#include "commands/AbstractCommand.h"
#include "objects/queue/QueueActionType.h"


enum class ActionType : char;
class SimulationObjectManager;

class UpgradeCommand : public AbstractCommand {
	friend class Stats;
public:
	UpgradeCommand(char player, int id, QueueActionType type);
	~UpgradeCommand() override;
	void execute() override;
	void setSimulationObjectManager(SimulationObjectManager* _simulationObjectManager);
private:
	QueueActionType type;
	short id;

	SimulationObjectManager* simulationObjectManager;
};
