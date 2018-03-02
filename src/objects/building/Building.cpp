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

double Building::hbMaxSize = 5.0;

Building::Building(Vector3* _position, int id, int player):target(*_position) , Static(_position, ObjectType::BUILDING) {
	hbMaxSize = 5.0;

	target.x_ += 5;
	target.z_ += 5;

	db_building* dbBuilding = Game::get()->getDatabaseCache()->getBuilding(id);
	std::vector<db_unit*>* dbUnits = Game::get()->getDatabaseCache()->getUnitsForBuilding(id);
	populate(dbBuilding, dbUnits);
	Model* model = Game::get()->getCache()->GetResource<Urho3D::Model>("Models/" + dbBuilding->model);
	Material* material = Game::get()->getCache()->GetResource<Urho3D::Material>("Materials/" + dbBuilding->texture);

	node->SetScale(dbBuilding->scale);
	Urho3D::StaticModel* staticModel = node->CreateComponent<Urho3D::StaticModel>();
	staticModel->SetModel(model);
	staticModel->SetMaterial(material);

	setPlayer(player);
	setTeam(Game::get()->getPlayersManager()->getPlayer(player)->getTeam()); 

	initBillbords();

}


Building::~Building() {
	delete queue;
}

float Building::getHealthBarSize() {
	double healthBarSize = hbMaxSize;
	if (healthBarSize <= 0) { healthBarSize = 0; }
	return healthBarSize;
}

int Building::getDbID() {
	return dbBuilding->id;
}

void Building::populate(db_building* _dbBuilding, std::vector<db_unit*>* _units) {
	gridSize = _dbBuilding->size;
	dbBuilding = _dbBuilding;
	units = _units;
	queue = new QueueManager(_dbBuilding->queueMaxCapacity);
}

void Building::absorbAttack(double attackCoef) {
}

String& Building::toMultiLineString() {
	menuString = dbBuilding->name;
	menuString += "\nAtak: " + String(attackCoef);
	menuString += "\nObrona: " + String(defenseCoef);
	menuString += "\nZdrowie: " + String(hpCoef) + "/" + String(maxHpCoef);
	return menuString;
}

void Building::action(short id, ActionParameter& parameter) {
	Resources& resources = Game::get()->getPlayersManager()->getActivePlayer()->getResources();
	std::vector<db_cost*>* costs = Game::get()->getDatabaseCache()->getCostForUnit(id);

	if (resources.reduce(costs)) {
		queue->add(1, QueueType::UNIT, id);
	}
}

std::string Building::getColumns() {
	return Static::getColumns() +
		"target_x		INT     NOT NULL,"
		"target_z		INT     NOT NULL";
}

std::string Building::getValues(int precision) {
	int target_x = target.x_ * precision;
	int target_z = target.z_ * precision;
	return Static::getValues(precision)
		+ to_string(target_x) + ","
		+ to_string(target_z);
}

QueueElement* Building::updateQueue(float time) {
	return queue->update(time);
}

Vector3& Building::getTarget() {
	return target;//TODO target to nie to samo co gdzie sie maja pojawiac!
}

QueueManager* Building::getQueue() {
	return queue;
}
