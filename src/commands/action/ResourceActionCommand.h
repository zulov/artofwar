#pragma once
#include <vector>
#include "objects/Physical.h"
#include "commands/action/UnitActionCommand.h"

enum class ResourceActionType : char;
class ResourceEntity;

class ResourceActionCommand : public AbstractCommand {
	friend class Stats;
public:
	ResourceActionCommand(ResourceEntity* resource, ResourceActionType action, char player);
	ResourceActionCommand(std::vector<Physical*>* resources, ResourceActionType action, char player);

	bool expired() override;
	void clean() override;

	void execute() override;

private:
	ResourceActionType action;
	std::vector<ResourceEntity*> resources;
};
