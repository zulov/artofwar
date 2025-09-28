#include "ResourceEntity.h"

#include <Urho3D/Resource/Localization.h>
#include <Urho3D/Scene/Node.h>
#include "Game.h"
#include "commands/action/ResourceActionType.h"
#include "database/DatabaseCache.h"
#include "math/RandGen.h"
#include "objects/PhysicalUtils.h"
#include "objects/unit/Unit.h"
#include "objects/unit/order/enums/UnitAction.h"
#include "objects/unit/state/StateManager.h"
#include "scene/load/dbload_container.h"
#include "env/Environment.h"
#include "player/PlayersManager.h"


ResourceEntity::ResourceEntity(Urho3D::Vector3 _position, db_resource* db_resource, int indexInGrid, UId uid)
	: Static(_position, indexInGrid, uid) {
	dbResource = db_resource;
	state = StaticState::ALIVE;
	if (node) {
		loadXml("Objects/resources/" + dbResource->nodeName[RandGen::nextRand(RandIntType::RESOURCE_NODE,
		                                                                      dbResource->nodeName.Size())]);
		if (dbResource->rotatable) {
			node->SetRotation(Urho3D::Quaternion(0, RandGen::nextRand(RandFloatType::RESOURCE_ROTATION, 360.f), 0.0f));
		}
	}
	populate();
}

const Urho3D::IntVector2 ResourceEntity::getGridSize() const {
	return dbResource->size;
}

void ResourceEntity::populate() {
	Static::populate();
	invMaxHp = dbResource->invMaxHp;
	hp = dbResource->maxHp;
	dbId = dbResource->id;
}

unsigned short ResourceEntity::getMaxHpBarSize() const {
	return 1;
}

Urho3D::String ResourceEntity::getInfo() const {
	const auto l10n = Game::getLocalization();
	const auto bonus = bonuses[Game::getPlayersMan()->getActivePlayerID()] * dbResource->collectSpeed;
	return l10nFormat("info_resource", dbResource->name.CString(), (int)hp, dbResource->maxHp,
	                  asStringF(bonus, 1).c_str(), closeUsers, getMaxCloseUsers());
}

const Urho3D::String& ResourceEntity::getName() const {
	return dbResource->name;
}

std::string ResourceEntity::getValues(int precision) {
	return Static::getValues(precision);
}

unsigned char ResourceEntity::getMaxCloseUsers() const {
	return dbResource->maxUsers;
}

char ResourceEntity::getResourceId() const { return dbResource->resourceId; }

void ResourceEntity::action(ResourceActionType type, char player) {
	switch (type) {
	case ResourceActionType::COLLECT: {
		if (hasAnyFreeSpace()) {
			const auto neights = Game::getEnvironment()->getNeighboursFromSparseSamePlayer(this, 24, player);
			int k = 0;
			const char limit = belowCloseLimit();
			for (auto neight : *neights) {
				if (k < limit) {
					auto unit = static_cast<Unit*>(neight);
					if (isFreeWorker(unit)) {
						//TODO findPath?
						unit->toAction(this, UnitAction::COLLECT);
						//TODO add order?
						++k;
					}
				} else {
					break;
				}
			}
		}
	}
	break;
	case ResourceActionType::CANCEL:
		StateManager::changeState(this, StaticState::FREE);
		break;
	}
}

bool ResourceEntity::canUse(int index) const {
	return Game::getEnvironment()->cellIsCollectable(index);
}

float ResourceEntity::getModelHeight() const {
	return dbResource->modelHeight;
}

void ResourceEntity::setModelData(float modelHeight) const {
	dbResource->modelHeight = modelHeight;
}

std::pair<float, bool> ResourceEntity::absorbAttack(float collectSpeed) {
	collectSpeed *= dbResource->collectSpeed;
	if (hp - collectSpeed > 0) {
		hp -= collectSpeed;
		return {collectSpeed, false};
	}

	const float toReturn = hp;
	hp = 0.f;
	StateManager::changeState(this, StaticState::DEAD);
	return {toReturn, true};
}

ResourceEntity* ResourceEntity::load(dbload_resource* resource) {
	Static::load(resource);
	return this;
}
