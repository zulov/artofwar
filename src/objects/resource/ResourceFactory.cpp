#include "ResourceFactory.h"
#include "Game.h"
#include "objects/building/Building.h"

ResourceFactory::ResourceFactory() {
	resources = new std::vector<ResourceEntity *>();
	resources->reserve(DEFAULT_VECTOR_SIZE);
}


ResourceFactory::~ResourceFactory() {
	resources->clear();
	delete resources;
}

std::vector<ResourceEntity*>* ResourceFactory::create(int id, Vector3* center, IntVector2 _bucketCords) {
	resources->clear();

	Vector3* position = new Vector3(center->x_, center->y_, center->z_);

	ResourceEntity* entity = new ResourceEntity(position, id);
	entity->setBucketPosition(_bucketCords);
	resources->push_back(entity);

	return resources;
}
