#include "SimulationCommand.h"


SimulationCommand::SimulationCommand(int _number, BuildingType _buildingType, Vector3* _position, SpacingType _spacingType, int _player) {
	number = _number;
	position = _position;
	spacingType = _spacingType;
	player = _player;

	buildingType = _buildingType;
	objectType = ObjectType::BUILDING;
}

SimulationCommand::SimulationCommand(int _number, UnitType _unitType, Vector3* _position, SpacingType _spacingType, int _player) {
	number = _number;
	position = _position;
	spacingType = _spacingType;
	player = _player;

	unitType = _unitType;
	objectType = ObjectType::UNIT;
}

SimulationCommand::SimulationCommand(int _number, ResourceType _resourceType, Vector3* _position, SpacingType _spacingType, int _player) {
	number = _number;
	position = _position;
	spacingType = _spacingType;
	player = _player;

	resourceType = _resourceType;
	objectType = ObjectType::RESOURCE;
}

SimulationCommand::~SimulationCommand() {
	delete position;
}

void SimulationCommand::execute() {
	switch (objectType) {
	case ENTITY:
		break;
	case UNIT:
		simulationObjectManager->addUnits(number, unitType, position, spacingType, player);
		break;
	case BUILDING:
		simulationObjectManager->addBuildings(number, buildingType, position, spacingType, player);
		break;
	case RESOURCE:
		simulationObjectManager->addResources(number, resourceType, position, spacingType);
		break;

	}
}

void SimulationCommand::setSimulationObjectManager(SimulationObjectManager* _simulationObjectManager) {
	simulationObjectManager = _simulationObjectManager;
}
