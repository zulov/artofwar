#include "Building.h"
#include "ObjectEnums.h"
#include "db_strcut.h"

double Building::hbMaxSize = 5.0;

Building::Building(Vector3* _position, Urho3D::Node* _boxNode) : Physical(_position, _boxNode, BUILDING) {
	hbMaxSize = 5.0;
	
	target = new Vector3(*_position);
	target->x_ += 20;
	target->z_ += 20;
}


Building::~Building() {
}

double Building::getHealthBarSize() {
	double healthBarSize = (hbMaxSize);
	if (healthBarSize <= 0) { healthBarSize = 0; }
	return healthBarSize;
}

int Building::getSubType() {
	return buildingType;
}

int Building::getSubTypeId() {
	return dbBuilding->id;
}

void Building::populate(db_building* _dbBuilding, std::vector<db_unit*>* _units) {
	minimalDistance = _dbBuilding->minDist;
	buildingType = BuildingType(_dbBuilding->type);
	dbBuilding = _dbBuilding;
	units = _units;
	queue = new QueueManager(_dbBuilding->queueMaxCapacity);
}

void Building::absorbAttack(double attackCoef) {
}

bool Building::isInGrandient() {
	return inGradient;
}

void Building::setInGradinet(bool _inGradient) {
	inGradient = _inGradient;
}

String Building::toMultiLineString() {
	String msg = dbBuilding->name;
	msg += "\nAtak: " + String(attackCoef);
	msg += "\nObrona: " + String(defenseCoef);
	msg += "\nZdrowie: " + String(hpCoef) + "/" + String(maxHpCoef);
	return msg;
}

void Building::buttonAction(short id) {
	queue->add(1, UNIT, id);
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
