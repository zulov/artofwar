#pragma once
#include <vector>
#include "AiOrderData.h"

struct AiNode {
public:
	float getValueDiff() const;
	AiOrderData getOrder();
	void update(float value);

	AiNode(const std::string& name, float targetValue, AiOrderData data, const AiNode* parent)
		: name(name), targetValue(targetValue), data(data), parent(parent) {
	}

	AiNode& addChild(const std::string& name, float targetValue, AiOrderData type);

private:
	const std::string name;
	AiNode getBestChild();
	bool canBeDone();
	float currentValue;
	const float targetValue;
	const AiOrderData data;
	std::vector<AiNode> children;
	const AiNode* parent;
	//std::vector<BuildingCondition> buildingConditions;
	//std::vector<ResourceCondition> resourceConditions;
};

inline float AiNode::getValueDiff() const {
	return targetValue - currentValue;
}

inline AiOrderData AiNode::getOrder() {
	if (children.empty()) {
		return data;
	}
	std::vector<float> values;
	values.reserve(children.size());
	for (auto child : children) {
		values.push_back(child.getValueDiff());
	}
	auto indexBest = std::max_element(values.begin(), values.end()) - values.begin();
	return children.at(indexBest).getOrder();
}

inline void AiNode::update(float value) {
	currentValue += value;
}

inline AiNode& AiNode::addChild(const std::string& name, float targetValue, AiOrderData type) {
	return children.emplace_back(name, targetValue, type, this);
}
