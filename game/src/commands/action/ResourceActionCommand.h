#pragma once
#include <vector>
#include "commands/PhysicalCommand.h"

enum class ResourceActionType : char;
class ResourceEntity;
class Physical;

class ResourceActionCommand : public PhysicalCommand {
public:
	ResourceActionCommand(ResourceEntity* resource, ResourceActionType action, char player);
	ResourceActionCommand(const std::vector<Physical*>& resources, ResourceActionType action, char player);

	void execute() override;

private:
	ResourceActionType action;
	char player;
	std::vector<ResourceEntity*> resources;
};
