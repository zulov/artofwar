#pragma once
#include <vector>
#include "AiOrderType.h"

struct AiNode {
public:
	AiOrderType getBestChild();
	bool canBeDone();
private:
	float targetValue;
	float currentValue;
	AiOrderType type;
	std::vector<AiNode> children;
	//std::vector<BuildingCondition> buildingConditions;
	//std::vector<ResourceCondition> resourceConditions;
};
