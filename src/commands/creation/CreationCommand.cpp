#include "CreationCommand.h"
#include "ObjectEnums.h"
#include "simulation/SimulationObjectManager.h"


CreationCommand::CreationCommand(ObjectType type, int id, const Urho3D::Vector2& position, int player,
                                 const Urho3D::IntVector2& bucketCords, int level): id(id), number(number),
	player(player), level(level),
	position(position), objectType(type), bucketCords(bucketCords) {

}

CreationCommand::CreationCommand(ObjectType type, int number, int id, const Urho3D::Vector2& position, int player,
                                 int level): id(id), number(number), player(player), level(level),
	position(position), objectType(type) {
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
