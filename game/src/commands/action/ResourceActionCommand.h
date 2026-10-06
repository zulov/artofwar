#pragma once
#include <vector>
#include "commands/PhysicalCommand.h"

enum class ResourceActionType : unsigned char;
class ResourceEntity;
class Physical;
struct PendingCommandSaveData;

class ResourceActionCommand : public PhysicalCommand {
public:
	ResourceActionCommand(ResourceEntity* resource, ResourceActionType action, unsigned char playerId);
	ResourceActionCommand(const std::vector<Physical*>& resources, ResourceActionType action, unsigned char playerId);

	void execute() override;
	PendingCommandSaveData saveState(unsigned short order) const override;

private:
	std::vector<ResourceEntity*> resources;
	ResourceActionType action;
	unsigned char playerId;
};
