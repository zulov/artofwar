#include "ResourceEntity.h"

double ResourceEntity::hbMaxSize = 3.0;

ResourceEntity::ResourceEntity(Vector3* _position, Urho3D::Node* _node) : Physical(_position, _node, RESOURCE) {
	hbMaxSize = 3.0;
}

ResourceEntity::~ResourceEntity() {
	if (name) {
		delete name;
	}
}

int ResourceEntity::getSubTypeId() {
	return dbResource->id;
}

void ResourceEntity::populate(db_resource* _dbResource) {
	type = _dbResource->id;
	name = new String(_dbResource->name);
	minimalDistance = _dbResource->minDist;
	amonut = _dbResource->maxCapacity;
	maxUsers = _dbResource->maxUsers;
	dbResource = _dbResource;
}

bool ResourceEntity::isInGrandient() {
	return inGradient;
}

void ResourceEntity::setInGradinet(bool _inGradient) {
	inGradient = _inGradient;
}

double ResourceEntity::getHealthBarSize() {
	return hbMaxSize;
}

String* ResourceEntity::toMultiLineString() {
	(*menuString) = dbResource->name;
	(*menuString) += "\nZasobów: " + String(amonut);
	return menuString;
}

double ResourceEntity::collect(double collectSpeed) {
	if (amonut - collectSpeed > 0) {
		amonut -= collectSpeed;
		return collectSpeed;
	}
	amonut = 0;
	return amonut;
}
