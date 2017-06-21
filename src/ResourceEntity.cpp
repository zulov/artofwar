#include "ResourceEntity.h"


ResourceEntity::ResourceEntity(Vector3* _position, Urho3D::Node* _node) : Entity(_position, _node) {
	
}

ResourceEntity::~ResourceEntity() {
	if(name) {
		delete name;
	}
}

ObjectType ResourceEntity::getType() {
	return RESOURCE;
}

int ResourceEntity::getSubType() {
	return type;
}

void ResourceEntity::populate(db_resource* dbResource) {
	type = dbResource->id;
	name = new String(dbResource->name);
	minimalDistance = dbResource->minDist;
}
bool ResourceEntity::isInGrandient() {
	return inGradient;
}

void ResourceEntity::setInGradinet(bool _inGradient) {
	inGradient = _inGradient;
}