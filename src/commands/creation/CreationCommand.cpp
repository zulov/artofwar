#include "CreationCommand.h"
#include "simulation/SimulationObjectManager.h"
#include "ObjectEnums.h"

CreationCommand::CreationCommand(ObjectType type, int id, const Urho3D::Vector2& _position, int _player, const Urho3D::IntVector2&
                                 _bucketCords,
                                 int level) {
	position = _position;
	player = _player;
	this->id = id;
	objectType = type;
	bucketCords = _bucketCords;
	this->level = level;
}

CreationCommand::CreationCommand(ObjectType type, int _number, int id, const Urho3D::Vector2& _position, int _player,
                                 int level) {
	number = _number;
	position = _position;
	player = _player;
	this->id = id;
	objectType = type;
	this->level = level;
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
