#pragma once
#include "objects/Entity.h"
#include <vector>
#include "AbstractCommand.h"
#include "simulation/SimulationObjectManager.h"
#include "ObjectEnums.h"

class CreationCommand : public AbstractCommand
{
public:
	CreationCommand(ObjectType type, int id, Vector3* _position, int _player, IntVector2 _bucketCords);
	CreationCommand(ObjectType type, int _number, int id, Vector3* _position, int _player);
	virtual ~CreationCommand();
	void execute() override;
	void setSimulationObjectManager(SimulationObjectManager* _simulationObjectManager);
private:
	SimulationObjectManager* simulationObjectManager;
	int id;
	int number;
	int player;
	Vector3* position;
	ObjectType objectType;
	IntVector2 bucketCords;
};
