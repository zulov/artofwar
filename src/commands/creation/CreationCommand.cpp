#include "CreationCommand.h"
#include "objects/ObjectEnums.h"
#include "simulation/SimulationObjectManager.h"


CreationCommand::CreationCommand(ObjectType type, int id, const Urho3D::Vector2& position, char player,
                                 const Urho3D::IntVector2& bucketCords, int level): AbstractCommand(player), id(id),
                                                                                    number(0),
                                                                                    level(level),
                                                                                    position(position),
                                                                                    objectType(type),
                                                                                    bucketCords(bucketCords) {
}

CreationCommand::CreationCommand(ObjectType type, int number, int id, const Urho3D::Vector2& position,
                                 char player, int level): AbstractCommand(player),
                                                          id(id), number(number), level(level),
                                                          position(position), objectType(type) {
}

void CreationCommand::execute() {
	switch (objectType) {
	case ObjectType::UNIT:
		simulationObjectManager->addUnits(number, id, position, player, level);
		break;
	case ObjectType::BUILDING:
		simulationObjectManager->addBuilding(id, position, player, bucketCords, level);
		break;
	case ObjectType::RESOURCE:
		simulationObjectManager->addResource(id, position, bucketCords, level);
		break;
	}
}

void CreationCommand::setSimulationObjectManager(SimulationObjectManager* _simulationObjectManager) {
	simulationObjectManager = _simulationObjectManager;
}
