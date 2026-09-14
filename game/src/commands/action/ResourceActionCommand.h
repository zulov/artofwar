#pragma once
#include <vector>
#include "commands/PhysicalCommand.h"

enum class ResourceActionType : unsigned char;
class ResourceEntity;
class Physical;
struct PendingCommandSaveData;

class ResourceActionCommand : public PhysicalCommand {
public:
	ResourceActionCommand(ResourceEntity* resource, ResourceActionType action, char player);
	ResourceActionCommand(const std::vector<Physical*>& resources, ResourceActionType action, char player);

	void execute() override;
	PendingCommandSaveData saveState(unsigned short order) const override;

private:
	std::vector<ResourceEntity*> resources;
	ResourceActionType action;
	char player;
};
