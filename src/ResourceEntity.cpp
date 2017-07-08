#include "ResourceEntity.h"


ResourceEntity::ResourceEntity(Vector3* _position, Urho3D::Node* _node) : Physical(_position, _node, RESOURCE) {

}

ResourceEntity::~ResourceEntity() {
	if (name) {
		delete name;
	}
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
