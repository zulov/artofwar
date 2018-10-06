#include "Building.h"
#include "Game.h"
#include "ObjectEnums.h"
#include "database/DatabaseCache.h"
#include "objects/MenuAction.h"
#include "objects/unit/ActionParameter.h"
#include "objects/unit/state/StateManager.h"
#include "player/Player.h"
#include "player/PlayersManager.h"
#include "player/Resources.h"
#include <Urho3D/Graphics/Material.h>
#include <Urho3D/Graphics/StaticModel.h>
#include <Urho3D/Resource/ResourceCache.h>
#include <Urho3D/Resource/XMLFile.h>
#include <string>


Building::Building(Urho3D::Vector3* _position, int id, int player, int level, int mainCell):
	Static(_position, ObjectType::BUILDING, mainCell),
	target(_position->x_ + 5, _position->z_ + 5) {
	dbBuilding = Game::getDatabaseCache()->getBuilding(id);
	upgrade(level);

	units = Game::getDatabaseCache()->getUnitsForBuilding(id);

	setPlayer(player);
	setTeam(Game::getPlayersManager()->getPlayer(player)->getTeam());
}


Building::~Building() {
	delete queue;
}

float Building::getMaxHpBarSize() {
	return 2.0f;
}

int Building::getDbID() {
	return dbBuilding->id;
}

void Building::populate() {
	Static::populate(dbBuilding->size);
	queue = new QueueManager(dbLevel->queueMaxCapacity);
}

void Building::absorbAttack(float attackCoef) {
	hpCoef -= attackCoef * (1 - defenseCoef);

	updateHealthBar();

	if (hpCoef < 0) {
		StateManager::changeState(this, StaticState::DEAD);
	}
}

Urho3D::String& Building::toMultiLineString() {
	menuString = dbBuilding->name + " " + dbLevel->name;
	menuString += "\nAtak: " + Urho3D::String(attackCoef);
	menuString += "\nObrona: " + Urho3D::String(defenseCoef);
	menuString += "\nZdrowie: " + Urho3D::String(hpCoef) + "/" + Urho3D::String(maxHpCoef);
	return menuString;
}

void Building::action(char id, const ActionParameter& parameter) {
	Resources& resources = Game::getPlayersManager()->getActivePlayer()->getResources();

	switch (parameter.type) {
	case MenuAction::UNIT:
		{
		auto costs = Game::getDatabaseCache()->getCostForUnit(id);
		if (resources.reduce(costs)) {
			queue->add(1, parameter.type, id, 30);
		}
		}
		break;
	case MenuAction::UNIT_LEVEL:
		{
		int level = Game::getPlayersManager()->getActivePlayer()->getLevelForUnit(id) + 1;
		auto opt = Game::getDatabaseCache()->getCostForUnitLevel(id, level);
		if (opt.has_value()) {
			const auto costs = opt.value();
			if (resources.reduce(costs)) {
				queue->add(1, parameter.type, id, 1);
			}
		}
		}
		break;
	case MenuAction::UNIT_UPGRADE:
		{
		int level = Game::getPlayersManager()->getActivePlayer()->getLevelForUnitUpgradePath(id) + 1;
		auto opt = Game::getDatabaseCache()->getCostForUnitUpgrade(id, level);
		if (opt.has_value()) {
			const auto costs = opt.value();
			if (resources.reduce(costs)) {
				queue->add(1, parameter.type, id, 1);
			}
		}
		}
		break;
	}
}

void Building::upgrade(char level) {
	dbLevel = Game::getDatabaseCache()->getBuildingLevel(dbBuilding->id, level).value();

	node->RemoveAllChildren();
	node->LoadXML(Game::getCache()->GetResource<Urho3D::XMLFile
	                              >("Objects/buildings/" + dbLevel->nodeName + "/" + Urho3D::String(level+1) + ".xml")
	                              ->GetRoot());
	model = node->GetComponent<Urho3D::StaticModel>();

	node->SetVar("link", this);
	initBillboards();

	populate();
	updateBillboards();
}

void Building::load(dbload_building* dbloadBuilding) {
	Static::load(dbloadBuilding);
	target = {dbloadBuilding->target_x, dbloadBuilding->target_z};
}

std::string Building::getColumns() {
	return Static::getColumns() +
		"target_x		INT     NOT NULL,"
		"target_z		INT     NOT NULL";
}

std::string Building::getValues(int precision) {
	int target_x = target.x_ * precision;
	int target_y = target.y_ * precision;
	return Static::getValues(precision)
		+ std::to_string(target_x) + ","
		+ std::to_string(target_y);
}

int Building::getLevel() {
	return dbLevel->level;
}
