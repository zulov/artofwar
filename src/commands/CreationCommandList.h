#pragma once
#include "CommandList.h"
#include "simulation/SimulationObjectManager.h"
#include "CreationCommand.h"

class SimulationObjectManager;

class CreationCommandList : public CommandList
{
public:
	CreationCommandList(SimulationObjectManager* _simulationObjectManager);
	virtual ~CreationCommandList();
	bool addUnits(int _number, int id, Vector3* _position, int _player);
	bool addBuilding(int id, Vector3* _position, int _player);
	bool addResource(int id, Vector3* _position);
protected:
	void setParemeters(AbstractCommand* command) override;
private:
	SimulationObjectManager* simulationObjectManager;
};
