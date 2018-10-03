#include "CreationCommand.h"
#include "ObjectEnums.h"
#include "simulation/SimulationObjectManager.h"


CreationCommand::CreationCommand(ObjectType type, int id, const Urho3D::Vector2& position, int player,
                                 const Urho3D::IntVector2& bucketCords, int level): objectType(type), number(number),
	id(id), position(position),
	player(player), bucketCords(bucketCords), level(level) {

}

CreationCommand::CreationCommand(ObjectType type, int number, int id, const Urho3D::Vector2& position, int player,
                                 int level): objectType(type), number(number), id(id), position(position),
	player(player), level(level) {
}

CreationCommand::~CreationCommand() = default;

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
