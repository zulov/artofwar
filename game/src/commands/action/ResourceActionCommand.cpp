#include "ResourceActionCommand.h"
#include "math/VectorUtils.h"
#include "objects/resource/ResourceEntity.h"

ResourceActionCommand::ResourceActionCommand(ResourceEntity* resource, ResourceActionType action, char player)
	: action(action), player(player) {
	resources.emplace_back(resource);
}

ResourceActionCommand::ResourceActionCommand(const std::vector<Physical*>& resources, ResourceActionType action,
                                             char player) : action(action), player(player) {
	this->resources.reserve(resources.size());
	for (auto* resource : resources) {
		this->resources.emplace_back(reinterpret_cast<ResourceEntity*>(resource));
	}
}

void ResourceActionCommand::execute() {
	for (const auto resource : resources) {
		if(resource->isAlive()) {
			resource->action(action, player);
		}
	}
}
