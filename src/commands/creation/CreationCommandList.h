#pragma once
#include "simulation/SimulationObjectManager.h"
#include "commands/CommandList.h"

class SimulationObjectManager;

class CreationCommandList : public CommandList
{
public:
	CreationCommandList();
	virtual ~CreationCommandList();
	bool addUnits(int _number, int id, Vector3* _position, int _player);
	bool addBuilding(int id, Vector3* _position, int _player);
	bool addResource(int id, Vector3* _position);
	SimulationObjectManager* getManager();

protected:
	void setParemeters(AbstractCommand* command) override;
private:
	SimulationObjectManager* simulationObjectManager;
};
