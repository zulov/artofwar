#include "CreationCommand.h"
#include "scene/load/RuntimeSaveData.h"
#include "objects/ObjectEnums.h"
#include "objects/resource/ResourceEntity.h"
#include "simulation/SimulationObjectManager.h"


CreationCommand::CreationCommand(ObjectType type, unsigned short id, const Urho3D::UShortVector2& bucketCords)
	: bucketCords(bucketCords), id(id), objectType(type), player(-1) {}

CreationCommand::CreationCommand(ObjectType type, unsigned short id, const Urho3D::UShortVector2& bucketCords, char level,
                                 char player)
	: bucketCords(bucketCords), id(id), objectType(type), level(level), player(player) {}

CreationCommand::CreationCommand(ObjectType type, unsigned short id, const Urho3D::Vector2& position, char level, char player,
                                 unsigned number) : position(position), number(number),
                                                    id(id), objectType(type), level(level), player(player) {}

void CreationCommand::execute(SimulationObjectManager* simulationObjectManager) {
	switch (objectType) {
	case ObjectType::UNIT:
		simulationObjectManager->addUnits(number, id, position, level, player);
		break;
	case ObjectType::BUILDING:
		simulationObjectManager->addBuilding(id, bucketCords, level, player);
		break;
	case ObjectType::RESOURCE:
		auto res = simulationObjectManager->addResource(id, bucketCords);
		if (res && hp > 0.f) {
			res->hp = hp;
		}
		break;
	}
}

PendingCommandSaveData CreationCommand::saveState(unsigned short order) const {
	PendingCommandSaveData state;
	state.order = order;
	state.kind = PendingCommandKind::CREATION;
	state.action = static_cast<char>(objectType);
	state.id = id;
	state.player = player;
	state.level = level;
	state.number = number;
	state.hp = hp;
	if (objectType == ObjectType::UNIT) {
		state.x = position.x_;
		state.z = position.y_;
	} else {
		state.x = bucketCords.x_;
		state.z = bucketCords.y_;
	}
	return state;
}
