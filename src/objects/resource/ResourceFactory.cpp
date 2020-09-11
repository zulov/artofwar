#include "ResourceFactory.h"
#include "Game.h"
#include "ResourceEntity.h"
#include "database/DatabaseCache.h"
#include "scene/load/dbload_container.h"
#include "simulation/env/Environment.h"

ResourceEntity* ResourceFactory::create(int id, Urho3D::Vector2& center, Urho3D::IntVector2 _bucketCords,
                                        int level) {

	float y = Game::getEnvironment()->getGroundHeightAt(center.x_, center.y_);
	auto mainCell = Game::getEnvironment()->getIndex(_bucketCords.x_, _bucketCords.y_);
	return new ResourceEntity(Urho3D::Vector3(center.x_, y, center.y_), id, level, mainCell);
}

ResourceEntity* ResourceFactory::load(dbload_resource_entities* resource) {
	Urho3D::IntVector2 bucketCords(resource->buc_x, resource->buc_y);
	db_resource* db_resource = Game::getDatabase()->getResource(resource->id_db);

	auto center = Game::getEnvironment()->getValidPosition(db_resource->size, bucketCords);

	auto ress = create(resource->id_db, center, bucketCords, resource->level);

	ress->load(resource);

	return ress;
}
