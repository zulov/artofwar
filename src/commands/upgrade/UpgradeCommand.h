#pragma once
#include "commands/AbstractCommand.h"
#include "objects/MenuAction.h"


class SimulationObjectManager;

class UpgradeCommand :
	public AbstractCommand
{
public:
	UpgradeCommand(int _player, int _id, MenuAction _type);
	~UpgradeCommand() override;
	void execute() override;
	void setSimulationObjectManager(SimulationObjectManager* _simulationObjectManager);
private:
	SimulationObjectManager* simulationObjectManager;

	int player;
	int id;
	MenuAction type;

};
