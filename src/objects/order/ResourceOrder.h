#pragma once
#include <vector>
#include "objects/Physical.h"
#include "commands/action/UnitActionCommand.h"

enum class ResourceActionType : char;
class ResourceEntity;

class ResourceOrder : public UnitActionCommand {

	ResourceOrder(ResourceEntity* resource, ResourceActionType action, short id);
	ResourceOrder(std::vector<Physical*>* resources, ResourceActionType action, short id);

	bool expired() override;
	void clean() override;

	void execute() override;
	
private:
	std::vector<ResourceEntity*> resources;
};
