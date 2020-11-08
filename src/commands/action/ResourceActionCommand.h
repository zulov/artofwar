#pragma once
#include <vector>
#include "commands/AbstractCommand.h"

enum class ResourceActionType : char;
class ResourceEntity;
class Physical;

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
