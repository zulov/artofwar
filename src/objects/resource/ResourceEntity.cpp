#include "ResourceEntity.h"
#include <string>
#include "Game.h"
#include "commands/action/ResourceActionType.h"
#include "database/DatabaseCache.h"
#include "math/RandGen.h"
#include "objects/ObjectEnums.h"
#include "objects/unit/Unit.h"
#include "objects/unit/order/enums/UnitAction.h"
#include "objects/unit/state/StateManager.h"
#include "player/Player.h"
#include "simulation/env/Environment.h"


ResourceEntity::ResourceEntity(Urho3D::Vector3& _position,
                               int id, int level, int mainCell)
	: Static(_position, mainCell) {
	dbResource = Game::getDatabase()->getResource(id);;
	loadXml("Objects/resources/" + dbResource->nodeName[RandGen::nextRand(
		RandIntType::RESOURCE_NODE, dbResource->nodeName.Size())]);

	node->SetRotation(Urho3D::Quaternion(0, RandGen::nextRand(RandFloatType::RESOURCE_ROTATION, 360.f), 0.0f));
}

ResourceEntity::~ResourceEntity() = default;

short ResourceEntity::getId() {
	return dbResource->id;
}

void ResourceEntity::populate() {
	Static::populate(dbResource->size);

	amonut = dbResource->maxCapacity;
	maxCloseUsers = dbResource->maxUsers;
}

float ResourceEntity::getMaxHpBarSize() {
	return 1;
}

float ResourceEntity::getHealthPercent() const {
	return amonut / dbResource->maxCapacity;
}

Urho3D::String ResourceEntity::toMultiLineString() {
	auto l10n = Game::getLocalization();

	return Urho3D::String(dbResource->name)
	       .Append("\n" + l10n->Get("ml_res_1") + ": ").Append(Urho3D::String((int)amonut))
	       .Append("\n" + l10n->Get("ml_res_2") + ": ").Append(Urho3D::String((int)closeUsers))
	       .Append("/").Append(Urho3D::String((int)maxCloseUsers));
}

std::string ResourceEntity::getValues(int precision) {
	int amountI = amonut * precision;
	return Static::getValues(precision)
		+ std::to_string(amountI);
}

void ResourceEntity::action(ResourceActionType type, char player) {
	switch (type) {
	case ResourceActionType::COLLECT:
	{
		auto neights = Game::getEnvironment()->getNeighboursFromTeamEq(this, 24, player);
		int k = 0;
		char limit = belowCloseLimit();
		for (auto neight : *neights) {
			if (k < limit) {
				auto unit = dynamic_cast<Unit*>(neight);
				if (unit->getState() == UnitState::STOP && StateManager::checkChangeState(unit, UnitState::COLLECT)) {
					auto opt = this->getPosToUseWithIndex(unit);
					if (opt.has_value()) {
						auto [pos, distance, indexOfPos] = opt.value();
						unit->toAction(this, distance, indexOfPos, UnitAction::COLLECT, 24);
						//TODO add order?
						++k;
					}
				}
			} else {
				break;
			}
		}
	}
	break;
	case ResourceActionType::CANCEL:
		StateManager::changeState(this, StaticState::FREE);
		break;
	}

}

ObjectType ResourceEntity::getType() const {
	return ObjectType::RESOURCE;
}

Urho3D::String ResourceEntity::getBarMaterialName() {
	return "Materials/bar/bar_grey.xml";
}

Urho3D::String ResourceEntity::getShadowMaterialName() {
	return "Materials/select/select_grey_" + dbResource->name + ".xml";
}

std::string ResourceEntity::getColumns() {
	return Static::getColumns() +
		"amount		INT     NOT NULL";
}

float ResourceEntity::collect(float collectSpeed) {
	if (amonut - collectSpeed >= 0) {
		amonut -= collectSpeed;
		updateHealthBar();
		return collectSpeed;
	}
	const float toReturn = amonut;
	amonut = 0;
	StateManager::changeState(this, StaticState::DEAD);
	return toReturn;
}

void ResourceEntity::load(dbload_resource_entities* resource) {
	Static::load(resource);
	this->amonut = resource->amount;
}
