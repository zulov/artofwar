#include "ResourceFactory.h"
#include "Game.h"
#include "scene/load/dbload_container.h"
#include "database/DatabaseCache.h"
#include "simulation/env/Enviroment.h"

ResourceFactory::ResourceFactory() {
	resources = new std::vector<ResourceEntity *>();
	resources->reserve(DEFAULT_VECTOR_SIZE);
}


ResourceFactory::~ResourceFactory() {
	delete resources;
}

std::vector<ResourceEntity*>* ResourceFactory::create(int id, Vector3* center, IntVector2 _bucketCords, int level) {
	resources->clear();

	Vector3* position = new Vector3(center->x_, center->y_, center->z_);

	ResourceEntity* entity = new ResourceEntity(position, id, level);
	entity->setBucketPosition(_bucketCords);
	resources->push_back(entity);

	return resources;
}

std::vector<ResourceEntity*>* ResourceFactory::load(dbload_resource_entities* resource) {
	resources->clear();

	Enviroment* env = Game::get()->getEnviroment();

	IntVector2 bucketCords(resource->buc_x, resource->buc_y);
	db_resource* db_resource = Game::get()->getDatabaseCache()->getResource(resource->id_db);

	Vector3* position = env->getValidPosition(db_resource->size, bucketCords);

	ResourceEntity* entity = new ResourceEntity(position, resource->id_db, resource->level);
	entity->setBucketPosition(bucketCords);
	resources->push_back(entity);

	return resources;
}
