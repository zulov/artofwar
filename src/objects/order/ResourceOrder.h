#pragma once
#include "FutureOrder.h"

enum class ResourceActionType : char;
class ResourceEntity;

class ResourceOrder : public FutureOrder {

	ResourceOrder(ResourceEntity* resource, ResourceActionType action, unsigned short id);
	ResourceOrder(std::vector<Physical*>* resources, ResourceActionType action, unsigned short id);

};
