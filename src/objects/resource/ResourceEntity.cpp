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

String *ResourceEntity::toMultiLineString() {
	(*menuString) = dbResource->name;
	(*menuString) += "\nZasob�w: " + String(amonut);
	return menuString;
}