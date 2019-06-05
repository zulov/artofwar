#include "ResourceFactory.h"
#include "Game.h"
#include "ResourceEntity.h"
#include "database/DatabaseCache.h"
#include "scene/load/dbload_container.h"
#include "simulation/env/Environment.h"

ResourceFactory::ResourceFactory() {
	resources = new std::vector<ResourceEntity *>();
}


ResourceFactory::~ResourceFactory() {
	delete resources;
}

std::vector<ResourceEntity*>* ResourceFactory::create(int id, Urho3D::Vector2& center, Urho3D::IntVector2 _bucketCords,
                                                      int level) const {
	resources->clear();

	float y = Game::getEnvironment()->getGroundHeightAt(center.x_, center.y_);
	auto mainCell = Game::getEnvironment()->getIndex(_bucketCords.x_, _bucketCords.y_);
	resources->push_back(new ResourceEntity(Urho3D::Vector3(center.x_, y, center.y_), id, level, mainCell));

	return resources;
}

std::vector<ResourceEntity*>* ResourceFactory::load(dbload_resource_entities* resource) const {
	Urho3D::IntVector2 bucketCords(resource->buc_x, resource->buc_y);
	db_resource* db_resource = Game::getDatabaseCache()->getResource(resource->id_db);

	auto center = Game::getEnvironment()->getValidPosition(db_resource->size, bucketCords);

	auto ress = create(resource->id_db, center, bucketCords, resource->level);
	for (auto res : *ress) {
		res->load(resource);
	}
	return ress;
}
