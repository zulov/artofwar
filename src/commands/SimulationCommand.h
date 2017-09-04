#pragma once
#include "Entity.h"
#include <vector>
#include "AbstractCommand.h"
#include "SimulationObjectManager.h"
#include "ObjectEnums.h"

class SimulationCommand:public AbstractCommand
{
public:
	SimulationCommand(ObjectType type, int _number, int id, Vector3* _position, SpacingType _spacingType, int _player);
	virtual ~SimulationCommand();
	void execute() override;
	void setSimulationObjectManager(SimulationObjectManager* _simulationObjectManager);
private:
	SimulationObjectManager* simulationObjectManager;
	int id;
	int number;
	int player;
	Vector3* position;
	ObjectType objectType;
	SpacingType spacingType;
};
