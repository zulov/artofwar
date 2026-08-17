#pragma once
#include "AiHistoryEnums.h"
#include <array>
#include <cstddef>
#include <initializer_list>

#include <magic_enum.hpp>

struct ActionHistoryEntry {
	unsigned int tick;
	AiActionType actionType;
	AiActionResult result;
	uint8_t chosenId;
};

struct OrderHistoryEntry {
	unsigned int tick;
	AiOrderType orderType;
	AiOrderResult result;
	uint8_t unitCount;
};

class AiHistory {
public:
	static constexpr int MAX_ENTRIES = 2048;

	void addAction(AiActionType type, AiActionResult result, uint8_t chosenId = 0);
	void addOrder(AiOrderType type, AiOrderResult result, uint8_t unitCount = 0);

	int getActionCount() const { return actionCount; }
	int getOrderCount() const { return orderCount; }

	float recencyScore(AiActionType type) const;
	float recencyScore(AiOrderType type) const;
	float recencyScore(std::initializer_list<AiActionType> types) const;
	float recencyScore(std::initializer_list<AiOrderType> types) const;
	float failureScore(AiActionType type) const;
	float failureScore(std::initializer_list<AiActionType> types) const;
	float failureScore(std::initializer_list<AiOrderType> types) const;

	float buildingFailureScore() const;
	float unitFailureScore() const;
	float collectFailureScore() const;
	float attackFailureScore() const;
	float defendFailureScore() const;
	float attackActivityScore() const;
	float defendActivityScore() const;

private:
	const ActionHistoryEntry& getAction(int index) const;
	const OrderHistoryEntry& getOrder(int index) const;
	void ensureScores() const;

	std::array<ActionHistoryEntry, MAX_ENTRIES> actions{};
	std::array<OrderHistoryEntry, MAX_ENTRIES> orders{};
	int actionHead = 0, actionCount = 0;
	int orderHead = 0, orderCount = 0;

	mutable std::array<float, magic_enum::enum_count<AiActionType>()> actionSuccessScores{};
	mutable std::array<float, magic_enum::enum_count<AiActionType>()> actionFailureScores{};
	mutable std::array<float, magic_enum::enum_count<AiOrderType>()> orderSuccessScores{};
	mutable std::array<float, magic_enum::enum_count<AiOrderType>()> orderFailureScores{};
};
