#pragma once
#include "commands/AbstractCommand.h"


enum class MenuAction : char;
class SimulationObjectManager;

class UpgradeCommand :
	public AbstractCommand
{
public:
	UpgradeCommand(int player, int id, MenuAction type);
	~UpgradeCommand() override;
	void execute() override;
	void setSimulationObjectManager(SimulationObjectManager* _simulationObjectManager);
private:
	SimulationObjectManager* simulationObjectManager;

	int player;
	int id;
	MenuAction type;

};
