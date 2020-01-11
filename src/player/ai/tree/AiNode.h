#pragma once
#include <vector>
#include "AiOrderData.h"
#include <optional>
#include "utils.h"

struct AiNode {

	AiNode(const std::string& name, float targetValue, AiOrderData data, const AiNode* parent)
		: name(name), currentValue(0), targetValue(targetValue), data(data), parent(parent) {
	}

	AiNode(const AiNode&) = delete;
	~AiNode() {
		clear_vector(children);
	}

	float getValueDiff() const;
	AiOrderData getOrder();
	void update(float value);
	const std::string& getName() const;
	std::optional<AiNode*> getChildByName(std::string str);

	AiNode* addChild(const std::string& name, float targetValue, AiOrderData data);

private:
	const std::string name;
	AiNode getBestChild();
	bool canBeDone();
	float currentValue;
	const float targetValue;
	const AiOrderData data;
	std::vector<AiNode*> children;
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
		values.push_back(child->getValueDiff());
	}
	auto indexBest = std::max_element(values.begin(), values.end()) - values.begin();
	return children.at(indexBest)->getOrder();
}

inline void AiNode::update(float value) {
	currentValue += value;
}

inline const std::string& AiNode::getName() const {
	return name;
}

inline std::optional<AiNode*> AiNode::getChildByName(std::string str) {
	for (auto& child : children) {
		if (child->getName() == str) {
			return child;
		}
	}
	return {};
}

inline AiNode* AiNode::addChild(const std::string& name, float targetValue, AiOrderData data) {
	children.emplace_back(new AiNode(name, targetValue, data, this));
	return children.back();
}
