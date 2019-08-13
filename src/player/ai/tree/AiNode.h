#pragma once
#include "AiOrderType.h"
#include <vector>

struct AiNode {
public:
	float getValueDiff() const;
	AiOrderType getOrder();
	void update(float value);
private:
	AiNode getBestChild();
	bool canBeDone();
	float currentValue;
	float targetValue;
	AiOrderType type;
	std::vector<AiNode> children;
	AiNode* parent;
	//std::vector<BuildingCondition> buildingConditions;
	//std::vector<ResourceCondition> resourceConditions;
};

inline float AiNode::getValueDiff() const {
	return targetValue - currentValue;
}

inline AiOrderType AiNode::getOrder() {
	if (children.empty()) {
		return type;
	} else {
		std::vector<float> values;
		values.reserve(children.size());
		for (auto child : children) {
			values.push_back(child.getValueDiff());
		}
		auto indexBest = std::max_element(values.begin(), values.end()) - values.begin();
		return children.at(indexBest).getOrder();
	}
}

inline void AiNode::update(float value) {
	currentValue += value;
}
