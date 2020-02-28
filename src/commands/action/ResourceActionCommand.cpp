#include "ResourceActionCommand.h"
#include "VectorUtils.h"
#include "consts.h"

ResourceActionCommand::ResourceActionCommand(ResourceEntity* resource, ResourceActionType action, char player)
	: AbstractCommand(player), action(action) {
	resources.emplace_back(resource);
}

ResourceActionCommand::ResourceActionCommand(std::vector<Physical*>* resources, ResourceActionType action, char player)
	: AbstractCommand(player), action(action) {
	for (auto resource : *resources) {
		//TODO performance spróbowaæ z insertem
		this->resources.emplace_back(reinterpret_cast<ResourceEntity*>(resource));
	}
}

void ResourceActionCommand::execute() {
	for (auto resource : resources) {
		resource->action(action);
	}
}

void ResourceActionCommand::clean() {
	cleanDead(resources);
}

bool ResourceActionCommand::expired() {
	return resources.empty();
}
