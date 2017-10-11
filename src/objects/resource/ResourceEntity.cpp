#include "ResourceEntity.h"

double ResourceEntity::hbMaxSize = 3.0;

ResourceEntity::ResourceEntity(Vector3* _position, Urho3D::Node* _node) : Static(_position, _node, RESOURCE) {
	hbMaxSize = 3.0;
}

ResourceEntity::~ResourceEntity() {
	if (name) {
		delete name;
	}
}

int ResourceEntity::getID() {
	return dbResource->id;
}

void ResourceEntity::populate(db_resource* _dbResource) {
	type = _dbResource->id;
	name = new String(_dbResource->name);
	gridSize = IntVector2(_dbResource->sizeX, _dbResource->sizeZ);
	amonut = _dbResource->maxCapacity;
	maxUsers = _dbResource->maxUsers;
	dbResource = _dbResource;
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
	if (amonut - collectSpeed >= 0) {
		amonut -= collectSpeed;
		return collectSpeed;
	}
	const double toReturn = amonut;
	amonut = 0;
	return toReturn;
}

bool ResourceEntity::belowLimit() {
	return users < maxUsers;
}

void ResourceEntity::up() {
	++users;
}

void ResourceEntity::reduce() {
	--users;
}
