#include "ResourceFactory.h"
#include "Game.h"
#include "ResourceEntity.h"
#include "database/DatabaseCache.h"
#include "scene/load/dbload_container.h"
#include "env/Environment.h"

ResourceEntity* ResourceFactory::create(int id, Urho3D::IntVector2 bucketCords, int level) const {
	db_resource* db_resource = Game::getDatabase()->getResource(id);
	if (Game::getEnvironment()->validateStatic(db_resource->size, bucketCords, false)) {
		auto center = Game::getEnvironment()->getValidPosition(db_resource->size, bucketCords);
		float y = Game::getEnvironment()->getGroundHeightAt(center.x_, center.y_);
		return new ResourceEntity(Urho3D::Vector3(center.x_, y, center.y_), id, level,
		                          Game::getEnvironment()->getIndex(bucketCords.x_, bucketCords.y_),
		                          !SIM_GLOBALS.HEADLESS);
	}
	return nullptr;
}

ResourceEntity* ResourceFactory::load(dbload_resource_entities* resource) const {
	const Urho3D::IntVector2 bucketCords(resource->buc_x, resource->buc_y);
	auto ress = create(resource->id_db, bucketCords, resource->level);
	if (ress) {
		return ress->load(resource);
	}
	return nullptr;
}
