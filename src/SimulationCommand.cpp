#include "SimulationCommand.h"


SimulationCommand::SimulationCommand(ObjectType type, int _number, int id, Vector3* _position, SpacingType _spacingType, int _player) {
	number = _number;
	position = _position;
	spacingType = _spacingType;
	player = _player;
	this->id = id;
	objectType = type;
}

SimulationCommand::~SimulationCommand() {
	delete position;
}

void SimulationCommand::execute() {
	switch (objectType) {
	case ENTITY:
		break;
	case UNIT:
		simulationObjectManager->addUnits(number, id, position, spacingType, player);
		break;
	case BUILDING:
		simulationObjectManager->addBuildings(number, id, position, spacingType, player);
		break;
	case RESOURCE:
		simulationObjectManager->addResources(number, id, position, spacingType);
		break;

	}
}

void SimulationCommand::setSimulationObjectManager(SimulationObjectManager* _simulationObjectManager) {
	simulationObjectManager = _simulationObjectManager;
}
