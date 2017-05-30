#include "SimulationCommand.h"


SimulationCommand::SimulationCommand(int _number, BuildingType _buildingType, Vector3* _position, SpacingType _spacingType, int _player) {
	number = _number;
	buildingType = _buildingType;
	position = _position;
	position->y_ = 0;
	spacingType = _spacingType;
	objectType = ObjectType::BUILDING;
	player = _player;
}

SimulationCommand::SimulationCommand(int _number, UnitType _unitType, Vector3* _position, SpacingType _spacingType, int _player) {
	number = _number;
	unitType = _unitType;
	position = _position;
	spacingType = _spacingType;
	objectType = ObjectType::UNIT;
	player = _player;
}

SimulationCommand::~SimulationCommand() {
}

void SimulationCommand::execute() {
	switch (objectType) {
	case ENTITY:
		break;
	case UNIT:
		simulationObjectManager->addUnits(number, unitType, position, spacingType, player);
		break;
	case BUILDING:
		simulationObjectManager->addBuildings(number, buildingType, position, spacingType);
		break;
	case RESOURCE: break;

	}
}

void SimulationCommand::setSimulationObjectManager(SimulationObjectManager* _simulationObjectManager) {
	simulationObjectManager = _simulationObjectManager;
}
