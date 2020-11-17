#include "ResourceActionCommand.h"
#include "math/VectorUtils.h"
#include "objects/resource/ResourceEntity.h"

ResourceActionCommand::ResourceActionCommand(ResourceEntity* resource, ResourceActionType action, char player)
	: AbstractCommand(player), action(action) {
	resources.emplace_back(resource);
}

ResourceActionCommand::ResourceActionCommand(const std::vector<Physical*>& resources, ResourceActionType action,
                                             char player) : AbstractCommand(player), action(action) {
	this->resources.reserve(resources.size());
	for (auto* resource : resources) {
		this->resources.emplace_back(reinterpret_cast<ResourceEntity*>(resource));
	}
}

void ResourceActionCommand::execute() {
	for (auto resource : resources) {
		resource->action(action, player);
	}
}

void ResourceActionCommand::clean() {
	cleanDead(resources);
}

bool ResourceActionCommand::expired() {
	return resources.empty();
}
