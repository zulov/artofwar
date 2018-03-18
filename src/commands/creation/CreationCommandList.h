#pragma once
#include "commands/CommandList.h"
#include "simulation/SimulationObjectManager.h"


class SimulationObjectManager;

class CreationCommandList : public CommandList
{
public:
	CreationCommandList();
	virtual ~CreationCommandList();
	bool addUnits(int _number, int id, Vector3& _position, int _player, int level);
	bool addBuilding(int id, Vector2& _position, int _player, int level);
	bool addResource(int id, Vector2& _position, int level);
	SimulationObjectManager* getManager();

protected:
	void setParemeters(AbstractCommand* command) override;
private:
	SimulationObjectManager* simulationObjectManager;
};
