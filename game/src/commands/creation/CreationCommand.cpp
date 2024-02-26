#include "CreationCommand.h"
#include "objects/ObjectEnums.h"
#include "simulation/SimulationObjectManager.h"


CreationCommand::CreationCommand(ObjectType type, short id, const Urho3D::IntVector2 bucketCords) 
	: bucketCords(bucketCords), objectType(type), id(id), player(-1) {}

CreationCommand::CreationCommand(ObjectType type, short id, const Urho3D::IntVector2 bucketCords, char level, char player)
	: bucketCords(bucketCords), objectType(type), id(id), level(level), player(player) {}

CreationCommand::CreationCommand(ObjectType type, short id, const Urho3D::Vector2& position, char level, char player,
                                 int number): position(position), number(number),
                                              objectType(type), id(id), level(level), player(player) {}

void CreationCommand::execute(SimulationObjectManager* simulationObjectManager) {
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
