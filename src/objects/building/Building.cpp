#include "Building.h"
#include "ObjectEnums.h"
#include "database/db_strcut.h"
#include "Game.h"

double Building::hbMaxSize = 5.0;

Building::Building(Vector3* _position, Urho3D::Node* _boxNode) : Static(_position, _boxNode, BUILDING) {
	hbMaxSize = 5.0;

	target = new Vector3(*_position);
	target->x_ += 5;
	target->z_ += 5;

}


Building::~Building() {
	delete queue;
}

double Building::getHealthBarSize() {
	double healthBarSize = (hbMaxSize);
	if (healthBarSize <= 0) { healthBarSize = 0; }
	return healthBarSize;
}

int Building::getID() {
	return dbBuilding->id;
}

void Building::populate(db_building* _dbBuilding, std::vector<db_unit*>* _units) {
	gridSize = IntVector2(_dbBuilding->minDist, _dbBuilding->minDist);
	buildingType = BuildingType(_dbBuilding->type);
	dbBuilding = _dbBuilding;
	units = _units;
	queue = new QueueManager(_dbBuilding->queueMaxCapacity);
}

void Building::absorbAttack(double attackCoef) {
}

String* Building::toMultiLineString() {
	(*menuString) = dbBuilding->name;
	(*menuString) += "\nAtak: " + String(attackCoef);
	(*menuString) += "\nObrona: " + String(defenseCoef);
	(*menuString) += "\nZdrowie: " + String(hpCoef) + "/" + String(maxHpCoef);
	return menuString;
}

void Building::action(short id, ActionParameter* parameter) {
	Resources* resources = Game::get()->getPlayersManager()->getActivePlayer()->getResources();
	std::vector<db_cost*>* costs = Game::get()->getDatabaseCache()->getCostForUnit(id);

	if (resources->reduce(costs)) {
		queue->add(1, UNIT, id);
	}
}

QueueElement* Building::updateQueue(float time) {
	return queue->update(time);
}

Vector3* Building::getTarget() {
	return target;//TODO target to nie to samo co gdzie sie maja pojawiac!
}

QueueManager* Building::getQueue() {
	return queue;
}
