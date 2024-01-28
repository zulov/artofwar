#include "ResourceFactory.h"
#include "Game.h"
#include "ResourceEntity.h"
#include "database/DatabaseCache.h"
#include "scene/load/dbload_container.h"
#include "env/Environment.h"

ResourceEntity* ResourceFactory::create(int id, Urho3D::IntVector2 bucketCords) const {
	db_resource* db_resource = Game::getDatabase()->getResource(id);
	const auto env = Game::getEnvironment();
	if (env->validateStatic(db_resource->size, bucketCords, false)) {
		const auto center = env->getValidPosition(db_resource->size, bucketCords);

		return new ResourceEntity(Urho3D::Vector3(center.x_,
		                                          env->getGroundHeightAt(center.x_, center.y_),
		                                          center.y_), db_resource,
		                          env->getIndex(bucketCords.x_, bucketCords.y_),
		                          !SIM_GLOBALS.HEADLESS);
	}
	return nullptr;
}

ResourceEntity* ResourceFactory::load(dbload_resource_entities* resource) const {
	const Urho3D::IntVector2 bucketCords(resource->buc_x, resource->buc_y);
	auto ress = create(resource->id_db, bucketCords);
	if (ress) {
		return ress->load(resource);
	}
	return nullptr;
}
