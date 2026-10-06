#include "CreationCommand.h"

#include <limits>

#include "scene/load/RuntimeSaveData.h"
#include "objects/ObjectEnums.h"
#include "objects/resource/ResourceEntity.h"
#include "simulation/SimulationObjectManager.h"


CreationCommand::CreationCommand(ObjectType type, unsigned short id, const Urho3D::UShortVector2& bucketCords)
	: bucketCords(bucketCords), id(id), objectType(type), playerId(std::numeric_limits<unsigned char>::max()) {}

CreationCommand::CreationCommand(ObjectType type, unsigned short id, const Urho3D::UShortVector2& bucketCords, char level,
                                 unsigned char playerId)
	: bucketCords(bucketCords), id(id), objectType(type), level(level), playerId(playerId) {}

CreationCommand::CreationCommand(ObjectType type, unsigned short id, const Urho3D::Vector2& position, char level,
                                 unsigned char playerId, unsigned number) : position(position), number(number),
                                                    id(id), objectType(type), level(level), playerId(playerId) {}

void CreationCommand::execute(SimulationObjectManager* simulationObjectManager) {
	switch (objectType) {
	case ObjectType::UNIT:
		simulationObjectManager->addUnits(number, id, position, level, playerId);
		break;
	case ObjectType::BUILDING:
		simulationObjectManager->addBuilding(id, bucketCords, level, playerId);
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
	state.playerId = playerId;
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
