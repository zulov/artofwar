#pragma once
#include "ObjectEnums.h"
#include "commands/AbstractCommand.h"
#include "simulation/SimulationObjectManager.h"


class CreationCommand : public AbstractCommand
{
public:
	CreationCommand(ObjectType type, int id, Vector2 _position, int _player, IntVector2 _bucketCords, int level);
	CreationCommand(ObjectType type, int _number, int id, Vector2 _position, int _player, int level);
	virtual ~CreationCommand();
	void execute() override;
	void setSimulationObjectManager(SimulationObjectManager* _simulationObjectManager);
private:
	SimulationObjectManager* simulationObjectManager;
	int id;
	int number;
	int player;
	int level;
	Vector2 position;
	ObjectType objectType;
	IntVector2 bucketCords;
};
