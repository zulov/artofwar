#include "Building.h"
#include "Game.h"
#include "ObjectEnums.h"
#include "database/DatabaseCache.h"
#include "objects/MenuAction.h"
#include "objects/unit/ActionParameter.h"
#include "player/PlayersManager.h"
#include "player/Resources.h"
#include <Urho3D/Graphics/Material.h>
#include <Urho3D/Graphics/Model.h>
#include <Urho3D/Graphics/StaticModel.h>
#include <Urho3D/Resource/ResourceCache.h>
#include <string>
#include "player/Player.h"


Building::Building(Vector3* _position, int id, int player, int level, const IntVector2& _bucketCords): Static(_position, ObjectType::BUILDING),
	target(_position->x_, _position->z_) {

	initBillbords();
	target.x_ += 5;
	target.y_ += 5;
	setMainCell(_bucketCords);
	dbBuilding = Game::getDatabaseCache()->getBuilding(id);
	units = Game::getDatabaseCache()->getUnitsForBuilding(id);

	setPlayer(player);
	setTeam(Game::getPlayersManager()->getPlayer(player)->getTeam());

	node->CreateComponent<Urho3D::StaticModel>();
	upgrade(level);
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
}

String& Building::toMultiLineString() {
	menuString = dbBuilding->name + " " + dbLevel->name;
	menuString += "\nAtak: " + String(attackCoef);
	menuString += "\nObrona: " + String(defenseCoef);
	menuString += "\nZdrowie: " + String(hpCoef) + "/" + String(maxHpCoef);
	return menuString;
}

void Building::action(char id, ActionParameter& parameter) {
	Resources& resources = Game::getPlayersManager()->getActivePlayer()->getResources();

	switch (parameter.type) {
	case MenuAction::UNIT:
		{
		std::vector<db_cost*>* costs = Game::getDatabaseCache()->getCostForUnit(id);
		if (resources.reduce(costs)) {
			queue->add(1, parameter.type, id, 30);
		}
		}
		break;
	case MenuAction::UNIT_LEVEL:
		{
		int level = Game::getPlayersManager()->getActivePlayer()->getLevelForUnit(id) + 1;
		optional<std::vector<db_cost*>*> opt = Game::getDatabaseCache()->getCostForUnitLevel(id, level);
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
		optional<std::vector<db_cost*>*> opt = Game::getDatabaseCache()->getCostForUnitUpgrade(id, level);
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
	auto staticModel = node->GetComponent<StaticModel>();
	dbLevel = Game::getDatabaseCache()->getBuildingLevel(dbBuilding->id, level).value();

	populate();

	node->SetScale(dbLevel->scale);

	staticModel->SetModel(Game::getCache()->GetResource<Urho3D::Model>("Models/" + dbLevel->model));
	staticModel->SetMaterial(Game::getCache()->GetResource<Urho3D::Material>("Materials/" + dbLevel->texture));
	updateBillbords();
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
		+ to_string(target_x) + ","
		+ to_string(target_y);
}

int Building::getLevel() {
	return dbLevel->level;
}
