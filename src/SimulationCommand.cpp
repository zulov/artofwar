#include "SimulationCommand.h"


SimulationCommand::SimulationCommand(int _number, BuildingType _buildingType, Vector3* _position, SpacingType _spacingType) {
	number = _number;
	buildingType = _buildingType;
	position = _position;
	spacingType = _spacingType;
	objectType = ObjectType::BUILDING;
}

SimulationCommand::SimulationCommand(int _number, UnitType _unitType, Vector3* _position, SpacingType _spacingType) {
	number = _number;
	unitType = _unitType;
	position = _position;
	spacingType = _spacingType;
	objectType = ObjectType::UNIT;
}

SimulationCommand::~SimulationCommand() {
}

void SimulationCommand::execute() {
	switch (objectType) {
	case ENTITY:
		break;
	case UNIT:
		simulationObjectManager->addUnits(number, unitType, position, spacingType);
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
