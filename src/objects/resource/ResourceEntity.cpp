#include "ResourceEntity.h"
#include "Game.h"
#include "ObjectEnums.h"
#include "OperatorType.h"
#include "ResourceOrder.h"
#include "database/DatabaseCache.h"
#include "UnitOrder.h"
#include "objects/unit/Unit.h"
#include "objects/unit/state/StateManager.h"
#include "player/Player.h"
#include "player/PlayersManager.h"
#include "simulation/env/Environment.h"
#include <string>


ResourceEntity::
ResourceEntity(Urho3D::Vector3* _position, int id, int level, int mainCell)
	: Static(_position, mainCell) {
	dbResource = Game::getDatabaseCache()->getResource(id);
	loadXml("Objects/resources/" + dbResource->nodeName);

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
	return 2.5;
}

float ResourceEntity::getHealthPercent() const {
	return amonut / dbResource->maxCapacity;
}

Urho3D::String ResourceEntity::toMultiLineString() {
	return Urho3D::String(dbResource->name).Append(
		                                       "\nZasobów: ").Append(Urho3D::String((int)amonut)).Append(
		                                       "\nU¿ytkowników: ").Append(Urho3D::String((int)closeUsers))
	                                       .Append("/").Append(Urho3D::String((int)maxCloseUsers));
}

std::string ResourceEntity::getValues(int precision) {
	int amountI = amonut * precision;
	return Static::getValues(precision)
		+ std::to_string(amountI);
}

void ResourceEntity::action(char id, const ActionParameter& parameter) {
	switch (id) {
	case ResourceOrder::COLLECT:
	{
		auto neights = Game::getEnvironment()->getNeighboursFromTeam(this, 24,
		                                                             Game::getPlayersMan()
		                                                             ->getActivePlayer()->getTeam(),
		                                                             OperatorType::EQUAL);
		int k = 0;
		char limit = belowCloseLimit();
		for (auto neight : *neights) {
			if (k < limit) {
				auto unit = static_cast<Unit*>(neight);
				if (unit->getState() == UnitState::STOP && StateManager::checkChangeState(unit, UnitState::COLLECT)) {
					auto opt = this->getPosToUseWithIndex(unit);
					if (opt.has_value()) {
						auto [pos, distance, indexOfPos] = opt.value();
						unit->toAction(this, distance, indexOfPos, UnitOrder::COLLECT, 24);
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

ObjectType ResourceEntity::getType() const{
	return ObjectType::RESOURCE;
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
