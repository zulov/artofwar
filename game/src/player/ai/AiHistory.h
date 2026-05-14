#pragma once
#include "AiHistoryEnums.h"
#include <array>
#include <optional>
#include <vector>

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
	static constexpr int MAX_ENTRIES = 30;

	void addAction(AiActionType type, AiActionResult result, uint8_t chosenId = 0);
	void addOrder(AiOrderType type, AiOrderResult result, uint8_t unitCount = 0);

	const ActionHistoryEntry& getAction(int index) const;
	const OrderHistoryEntry& getOrder(int index) const;

	int getActionCount() const { return actionCount; }
	int getOrderCount() const { return orderCount; }

	std::vector<const ActionHistoryEntry*> getActionsInLastTicks(unsigned int ticks,
		std::optional<AiActionType> typeFilter = std::nullopt) const;
	std::vector<const OrderHistoryEntry*> getOrdersInLastTicks(unsigned int ticks,
		std::optional<AiOrderType> typeFilter = std::nullopt) const;

	float recencyScore(AiActionType type, unsigned int lookbackTicks) const;
	float recencyScore(AiOrderType type, unsigned int lookbackTicks) const;

private:
	std::array<ActionHistoryEntry, MAX_ENTRIES> actions{};
	std::array<OrderHistoryEntry, MAX_ENTRIES> orders{};
	int actionHead = 0, actionCount = 0;
	int orderHead = 0, orderCount = 0;
};
