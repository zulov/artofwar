#include "ResourceFactory.h"
#include "Game.h"
#include "ResourceEntity.h"
#include "database/DatabaseCache.h"
#include "scene/load/dbload_container.h"
#include "env/Environment.h"

ResourceFactory::ResourceFactory(unsigned currentUnitUId) : currentUId(currentUnitUId) {}

ResourceEntity* ResourceFactory::create(int id, Urho3D::IntVector2 bucketCords) {
	return create(id, bucketCords, UId(++currentUId));
}

ResourceEntity* ResourceFactory::load(dbload_resource_entities* resource) const {
	auto ress = create(resource->id_db, {resource->buc_x, resource->buc_y}, UId(resource->uid));
	if (ress) {
		return ress->load(resource);
	}
	return nullptr;
}

ResourceEntity* ResourceFactory::create(int id, Urho3D::IntVector2 bucketCords, UId uid) const {
	db_resource* db_resource = Game::getDatabase()->getResource(id);
	const auto env = Game::getEnvironment();
	if (env->validateStatic(db_resource->size, bucketCords, false)) {
		return new ResourceEntity(env->getValidPosition(db_resource->size, bucketCords), db_resource,
		                          env->getIndex(bucketCords.x_, bucketCords.y_), uid);
	}
	return nullptr;
}
