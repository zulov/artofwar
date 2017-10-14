#include "CreationCommand.h"


CreationCommand::CreationCommand(ObjectType type, int _number, int id, Vector3* _position, SpacingType _spacingType, int _player) {
	number = _number;
	position = _position;
	spacingType = _spacingType;
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
		simulationObjectManager->addUnits(number, id, position, spacingType, player);
		break;
	case BUILDING:
		simulationObjectManager->addBuildings(id, position, player);
		break;
	case RESOURCE:
		simulationObjectManager->addResources(number, id, position, spacingType);
		break;
	}
}

void CreationCommand::setSimulationObjectManager(SimulationObjectManager* _simulationObjectManager) {
	simulationObjectManager = _simulationObjectManager;
}
