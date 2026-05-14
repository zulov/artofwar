#include "AiHistory.h"
#include <algorithm>
#include "Game.h"

void AiHistory::addAction(AiActionType type, AiActionResult result, uint8_t chosenId) {
	actions[actionHead] = {Game::getFrameInfo()->getTotalTicks(), type, result, chosenId};
	actionHead = (actionHead + 1) % MAX_ENTRIES;
	actionCount = std::min(actionCount + 1, MAX_ENTRIES);
}

void AiHistory::addOrder(AiOrderType type, AiOrderResult result, uint8_t unitCount) {
	orders[orderHead] = {Game::getFrameInfo()->getTotalTicks(), type, result, unitCount};
	orderHead = (orderHead + 1) % MAX_ENTRIES;
	orderCount = std::min(orderCount + 1, MAX_ENTRIES);
}

const ActionHistoryEntry& AiHistory::getAction(int index) const {
	int actual = (actionHead - actionCount + index + MAX_ENTRIES) % MAX_ENTRIES;
	return actions[actual];
}

const OrderHistoryEntry& AiHistory::getOrder(int index) const {
	int actual = (orderHead - orderCount + index + MAX_ENTRIES) % MAX_ENTRIES;
	return orders[actual];
}

std::vector<const ActionHistoryEntry*> AiHistory::getActionsInLastTicks(unsigned int ticks,
	std::optional<AiActionType> typeFilter) const {
	const unsigned int now = Game::getFrameInfo()->getTotalTicks();
	const unsigned int minTick = now > ticks ? now - ticks : 0;
	std::vector<const ActionHistoryEntry*> result;
	for (int i = actionCount - 1; i >= 0; --i) {
		const auto& entry = getAction(i);
		if (entry.tick < minTick) { break; }
		if (!typeFilter.has_value() || entry.actionType == typeFilter.value()) {
			result.push_back(&entry);
		}
	}
	return result;
}

std::vector<const OrderHistoryEntry*> AiHistory::getOrdersInLastTicks(unsigned int ticks,
	std::optional<AiOrderType> typeFilter) const {
	const unsigned int now = Game::getFrameInfo()->getTotalTicks();
	const unsigned int minTick = now > ticks ? now - ticks : 0;
	std::vector<const OrderHistoryEntry*> result;
	for (int i = orderCount - 1; i >= 0; --i) {
		const auto& entry = getOrder(i);
		if (entry.tick < minTick) { break; }
		if (!typeFilter.has_value() || entry.orderType == typeFilter.value()) {
			result.push_back(&entry);
		}
	}
	return result;
}
