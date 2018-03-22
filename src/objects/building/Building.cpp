#include "Building.h"
#include "Game.h"
#include "ObjectEnums.h"
#include "database/DatabaseCache.h"
#include "database/db_strcut.h"
#include "player/PlayersManager.h"
#include "player/Resources.h"
#include <Urho3D/Graphics/Material.h>
#include <Urho3D/Graphics/Model.h>
#include <Urho3D/Graphics/StaticModel.h>
#include <Urho3D/Resource/ResourceCache.h>
#include <string>

float Building::hbMaxSize = 5.0;

Building::Building(Vector3* _position, int id, int player, int level): target(_position->x_, _position->z_),
	Static(_position, ObjectType::BUILDING) {

	initBillbords();
	hbMaxSize = 5.0;
	target.x_ += 5;
	target.y_ += 5;

	dbBuilding = Game::get()->getDatabaseCache()->getBuilding(id);
	units = Game::get()->getDatabaseCache()->getUnitsForBuilding(id);

	setPlayer(player);
	setTeam(Game::get()->getPlayersManager()->getPlayer(player)->getTeam());

	node->CreateComponent<Urho3D::StaticModel>();
	upgrade(level);
}


Building::~Building() {
	delete queue;
}

float Building::getHealthBarSize() {
	float healthBarSize = hbMaxSize;
	if (healthBarSize <= 0) { healthBarSize = 0; }
	return healthBarSize;
}

int Building::getDbID() {
	return dbBuilding->id;
}

void Building::populate() {
	gridSize = dbBuilding->size;
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

void Building::action(short id, ActionParameter& parameter) {
	Resources& resources = Game::get()->getPlayersManager()->getActivePlayer()->getResources();

	switch (parameter.type) {
	case ActionType::UNIT:
		{
		std::vector<db_cost*>* costs = Game::get()->getDatabaseCache()->getCostForUnit(id);
		if (resources.reduce(costs)) {
			queue->add(1, parameter.type, id, 30);
		}
		}
		break;
	case ActionType::UNIT_LEVEL:
		{
		int level = Game::get()->getPlayersManager()->getActivePlayer()->getLevelForUnit(id) + 1;
		optional<std::vector<db_cost*>*> opt = Game::get()->getDatabaseCache()->getCostForUnitLevel(id, level);
		if (opt.has_value()) {
			const auto costs = opt.value();
			if (resources.reduce(costs)) {
				queue->add(1, parameter.type, id, 1);
			}
		}
		}
		break;
	case ActionType::UNIT_UPGRADE:
		{
		int level = Game::get()->getPlayersManager()->getActivePlayer()->getLevelForUnitUpgradePath(id) + 1;
		optional<std::vector<db_cost*>*> opt = Game::get()->getDatabaseCache()->getCostForUnitUpgrade(id, level);
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
	StaticModel* staticModel = node->GetComponent<StaticModel>();
	dbLevel = Game::get()->getDatabaseCache()->getBuildingLevel(dbBuilding->id, level).value();

	populate();

	node->SetScale(dbLevel->scale);
	Model* model = Game::get()->getCache()->GetResource<Urho3D::Model>("Models/" + dbLevel->model);
	Material* material = Game::get()->getCache()->GetResource<Urho3D::Material>("Materials/" + dbLevel->texture);
	staticModel->SetModel(model);
	staticModel->SetMaterial(material);
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

QueueElement* Building::updateQueue(float time) {
	return queue->update(time);
}

Vector2& Building::getTarget() {
	return target; //TODO target to nie to samo co gdzie sie maja pojawiac!
}

QueueManager* Building::getQueue() {
	return queue;
}
