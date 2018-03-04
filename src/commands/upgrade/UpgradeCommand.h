#pragma once
#include "d:\Programowanie\vs2015\art_of_war\src\commands\AbstractCommand.h"
#include "objects/queue/QueueType.h"

class SimulationObjectManager;

class UpgradeCommand :
	public AbstractCommand
{
public:
	UpgradeCommand(int _player, int _id, QueueType _type);
	~UpgradeCommand() override;
	void execute() override;
	void setSimulationObjectManager(SimulationObjectManager* _simulationObjectManager);
private:
	SimulationObjectManager* simulationObjectManager;

	int player;
	int id;
	QueueType type;

};
