#include "ResourceFactory.h"
#include "ResourceEntity.h"
#include "Game.h"
#include "database/DatabaseCache.h"
#include "scene/load/dbload_container.h"
#include "simulation/env/Enviroment.h"

ResourceFactory::ResourceFactory() {
	resources = new std::vector<ResourceEntity *>();
	resources->reserve(DEFAULT_VECTOR_SIZE);
}


ResourceFactory::~ResourceFactory() {
	delete resources;
}

std::vector<ResourceEntity*>* ResourceFactory::create(int id, Vector2& center, IntVector2 _bucketCords, int level) {
	resources->clear();

	float y = Game::getEnviroment()->getGroundHeightAt(center.x_, center.y_);
	Vector3* position = new Vector3(center.x_, y, center.y_);

	ResourceEntity* entity = new ResourceEntity(position, id, level);
	entity->setBucketPosition(_bucketCords);
	resources->push_back(entity);

	return resources;
}

std::vector<ResourceEntity*>* ResourceFactory::load(dbload_resource_entities* resource) {
	IntVector2 bucketCords(resource->buc_x, resource->buc_y);
	db_resource* db_resource = Game::getDatabaseCache()->getResource(resource->id_db);

	Vector2 center = Game::getEnviroment()->getValidPosition(db_resource->size, bucketCords);

	return create(resource->id_db, center, bucketCords, resource->level);
}
