#include "CreationCommand.h"


CreationCommand::CreationCommand(ObjectType type, int id, Vector3* _position, int _player, IntVector2 _bucketCords) {
	position = _position;
	player = _player;
	this->id = id;
	objectType = type;
	bucketCords = _bucketCords;
}

CreationCommand::CreationCommand(ObjectType type, int _number, int id, Vector3* _position, int _player) {
	number = _number;
	position = _position;
	player = _player;
	this->id = id;
	objectType = type;
}

CreationCommand::~CreationCommand() {
	delete position;
}

void CreationCommand::execute() {
	switch (objectType) {
	case ENTITY:
		break;
	case UNIT:
		simulationObjectManager->addUnits(number, id, position, player);
		break;
	case BUILDING:
		simulationObjectManager->addBuildings(id, position, player, bucketCords);
		break;
	case RESOURCE:
		simulationObjectManager->addResources(id, position, bucketCords);
		break;
	}
}

void CreationCommand::setSimulationObjectManager(SimulationObjectManager* _simulationObjectManager) {
	simulationObjectManager = _simulationObjectManager;
}
