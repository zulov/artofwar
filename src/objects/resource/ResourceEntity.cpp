#include "ResourceEntity.h"
#include "Game.h"
#include "objects/ObjectEnums.h"
#include "ResourceOrder.h"
#include "database/DatabaseCache.h"
#include "UnitAction.h"
#include "objects/unit/Unit.h"
#include "objects/unit/state/StateManager.h"
#include "player/Player.h"
#include "player/PlayersManager.h"
#include "simulation/env/Environment.h"
#include <string>


ResourceEntity::ResourceEntity(Urho3D::Vector3& _position,
                               int id, int level, int mainCell)
	: Static(_position, mainCell) {
	dbResource = Game::getDatabase()->getResource(id);;
	loadXml("Objects/resources/" + dbResource->nodeName[rand() % dbResource->nodeName.Size()]);

	node->SetRotation(Urho3D::Quaternion(0, rand() % 360, 0.0f));
}

ResourceEntity::~ResourceEntity() = default;

int ResourceEntity::getDbID() {
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

void ResourceEntity::action(char id, const ActionParameter& parameter) {
	switch (static_cast<ResourceOrder>(id)) {
	case ResourceOrder::COLLECT:
	{
		auto neights = Game::getEnvironment()->getNeighboursFromTeamEq(this, 24,
		                                                             Game::getPlayersMan()
		                                                             ->getActivePlayer()->getTeam());
		int k = 0;
		char limit = belowCloseLimit();
		for (auto neight : *neights) {
			if (k < limit) {
				auto unit = static_cast<Unit*>(neight);
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
	case ResourceOrder::COLLECT_CANCEL:
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
