#include "CreationCommand.h"
#include "objects/ObjectEnums.h"
#include "simulation/SimulationObjectManager.h"


CreationCommand::CreationCommand(ObjectType type, int id, const Urho3D::IntVector2 bucketCords) 
	: AbstractCommand(-1), id(id), objectType(type), bucketCords(bucketCords) {}

CreationCommand::CreationCommand(ObjectType type, int id, const Urho3D::IntVector2 bucketCords, char level, char player)
	: AbstractCommand(player), id(id), level(level), objectType(type), bucketCords(bucketCords) {}

CreationCommand::CreationCommand(ObjectType type, int id, const Urho3D::Vector2& position, char level, char player,
                                 int number): AbstractCommand(player), id(id),
                                              number(number), level(level), position(position), objectType(type) {}

void CreationCommand::execute() {
	switch (objectType) {
	case ObjectType::UNIT:
		simulationObjectManager->addUnits(number, id, position, level, player);
		break;
	case ObjectType::BUILDING:
		simulationObjectManager->addBuilding(id, bucketCords, level, player);
		break;
	case ObjectType::RESOURCE:
		simulationObjectManager->addResource(id, bucketCords);
		break;
	}
}

void CreationCommand::setSimulationObjectManager(SimulationObjectManager* _simulationObjectManager) {
	simulationObjectManager = _simulationObjectManager;
}
