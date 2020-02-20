#include "ResourceOrder.h"
#include "VectorUtils.h"

ResourceOrder::ResourceOrder(ResourceEntity* resource, ResourceActionType action, short id)
	: FutureOrder(static_cast<short>(action), id, false) {
	resources.emplace_back(resource);
}

ResourceOrder::ResourceOrder(std::vector<Physical*>* resources, ResourceActionType action, short id)
	: FutureOrder(static_cast<short>(action), id, false) {
	for (auto resource : *resources) {
		//TODO performance spróbowaæ z insertem
		this->resources.emplace_back(reinterpret_cast<ResourceEntity*>(resource));
	}
}

void ResourceOrder::clean() {
	cleanDead(resources);
}


bool ResourceOrder::expired() {
	return resources.empty();
}
