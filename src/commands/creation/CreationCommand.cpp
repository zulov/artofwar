#include "CreationCommand.h"


CreationCommand::CreationCommand(ObjectType type, int id, Vector3* _position, int _player, IntVector2 _bucketCords,
                                 int level) {
	position = _position;
	player = _player;
	this->id = id;
	objectType = type;
	bucketCords = _bucketCords;
	this->level = level;
}

CreationCommand::CreationCommand(ObjectType type, int _number, int id, Vector3* _position, int _player, int level) {
	number = _number;
	position = _position;
	player = _player;
	this->id = id;
	objectType = type;
	this->level = level;
}

CreationCommand::~CreationCommand() {
	delete position;
}

void CreationCommand::execute() {
	switch (objectType) {
	case ObjectType::UNIT:
		simulationObjectManager->addUnits(number, id, position, player, level);
		break;
	case ObjectType::BUILDING:
		simulationObjectManager->addBuildings(id, position, player, bucketCords);
		break;
	case ObjectType::RESOURCE:
		simulationObjectManager->addResources(id, position, bucketCords);
		break;
	}
}

void CreationCommand::setSimulationObjectManager(SimulationObjectManager* _simulationObjectManager) {
	simulationObjectManager = _simulationObjectManager;
}
