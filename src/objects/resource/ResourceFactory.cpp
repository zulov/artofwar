#include "ResourceFactory.h"
#include "Game.h"
#include "ResourceEntity.h"
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

std::vector<ResourceEntity*>* ResourceFactory::create(int id, Urho3D::Vector2& center, Urho3D::IntVector2 _bucketCords, int level) const {
	resources->clear();

	float y = Game::getEnviroment()->getGroundHeightAt(center.x_, center.y_);

	resources->push_back(new ResourceEntity(new Urho3D::Vector3(center.x_, y, center.y_), id, level, _bucketCords));

	return resources;
}

std::vector<ResourceEntity*>* ResourceFactory::load(dbload_resource_entities* resource) const {
	Urho3D::IntVector2 bucketCords(resource->buc_x, resource->buc_y);
	db_resource* db_resource = Game::getDatabaseCache()->getResource(resource->id_db);

	auto center = Game::getEnviroment()->getValidPosition(db_resource->size, bucketCords);

	return create(resource->id_db, center, bucketCords, resource->level);
}
