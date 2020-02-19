#pragma once
#include "FutureOrder.h"

enum class ResourceActionType : char;
class ResourceEntity;

class ResourceOrder : public FutureOrder {

	ResourceOrder(ResourceEntity* resource, ResourceActionType action, short id);
	ResourceOrder(std::vector<Physical*>* resources, ResourceActionType action, short id);
	
	bool expired() override;
	bool clean() override;
private:
	std::vector<ResourceEntity*> resources;
};
