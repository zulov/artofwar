#include "AiHistory.h"
#include <algorithm>
#include "Game.h"

void AiHistory::addAction(AiActionType type, AiActionResult result, uint8_t chosenId) {
	actions[actionHead] = {Game::getFrameInfo()->getTotalTicks(), type, result, chosenId};
	// std::cout << magic_enum::enum_name(type) << " " << magic_enum::enum_name(result) << std::endl;
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
		if (!typeFilter.has_value() || entry.actionType == typeFilter.value()) { result.push_back(&entry); }
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
		if (!typeFilter.has_value() || entry.orderType == typeFilter.value()) { result.push_back(&entry); }
	}
	return result;
}

float AiHistory::recencyScore(AiActionType type, unsigned int lookbackTicks) const {
	const unsigned int now = Game::getFrameInfo()->getTotalTicks();
	const unsigned int minTick = now > lookbackTicks ? now - lookbackTicks : 0;
	float score = 0.f;
	for (int i = actionCount - 1; i >= 0; --i) {
		const auto& entry = getAction(i);
		if (entry.tick < minTick) { break; }
		if (entry.actionType == type && entry.result == AiActionResult::SUCCESS) {
			score += 1.f - static_cast<float>(now - entry.tick) / lookbackTicks;
		}
	}
	return score;
}

float AiHistory::recencyScore(std::initializer_list<AiActionType> types, unsigned int lookbackTicks) const {
	float score = 0.f;
	for (auto type : types) { score += recencyScore(type, lookbackTicks); }
	return score;
}

float AiHistory::recencyScore(std::initializer_list<AiOrderType> types, unsigned int lookbackTicks) const {
	float score = 0.f;
	for (auto type : types) { score += recencyScore(type, lookbackTicks); }
	return score;
}

float AiHistory::failureScore(std::initializer_list<AiActionType> types, unsigned int lookbackTicks) const {
	float score = 0.f;
	for (auto type : types) { score += failureScore(type, lookbackTicks); }
	return score;
}

float AiHistory::failureScore(std::initializer_list<AiOrderType> types, unsigned int lookbackTicks) const {
	float score = 0.f;
	for (auto type : types) { score += failureScore(type, lookbackTicks); }
	return score;
}

float AiHistory::recencyScore(AiOrderType type, unsigned int lookbackTicks) const {
	const unsigned int now = Game::getFrameInfo()->getTotalTicks();
	const unsigned int minTick = now > lookbackTicks ? now - lookbackTicks : 0;
	float score = 0.f;
	for (int i = orderCount - 1; i >= 0; --i) {
		const auto& entry = getOrder(i);
		if (entry.tick < minTick) { break; }
		if (entry.orderType == type && entry.result == AiOrderResult::SUCCESS) {
			score += 1.f - static_cast<float>(now - entry.tick) / lookbackTicks;
		}
	}
	return score;
}

float AiHistory::failureScore(AiActionType type, unsigned int lookbackTicks) const {
	const unsigned int now = Game::getFrameInfo()->getTotalTicks();
	const unsigned int minTick = now > lookbackTicks ? now - lookbackTicks : 0;
	float score = 0.f;
	for (int i = actionCount - 1; i >= 0; --i) {
		const auto& entry = getAction(i);
		if (entry.tick < minTick) { break; }
		if (entry.actionType == type && entry.result != AiActionResult::SUCCESS) {
			score += 1.f - static_cast<float>(now - entry.tick) / lookbackTicks;
		}
	}
	return score;
}

float AiHistory::failureScore(AiOrderType type, unsigned int lookbackTicks) const {
	const unsigned int now = Game::getFrameInfo()->getTotalTicks();
	const unsigned int minTick = now > lookbackTicks ? now - lookbackTicks : 0;
	float score = 0.f;
	for (int i = orderCount - 1; i >= 0; --i) {
		const auto& entry = getOrder(i);
		if (entry.tick < minTick) { break; }
		if (entry.orderType == type && entry.result != AiOrderResult::SUCCESS) {
			score += 1.f - static_cast<float>(now - entry.tick) / lookbackTicks;
		}
	}
	return score;
}

float AiHistory::buildingFailureScore(unsigned int lookbackTicks) const {
	return failureScore({AiActionType::CREATE_BUILDING}, lookbackTicks);
}

float AiHistory::unitFailureScore(unsigned int lookbackTicks) const {
	return failureScore(AiActionType::CREATE_UNIT, lookbackTicks);
}

float AiHistory::collectFailureScore(unsigned int lookbackTicks) const {
	return failureScore({
			                    AiOrderType::COLLECT_RESOURCE_0, AiOrderType::COLLECT_RESOURCE_1,
			                    AiOrderType::COLLECT_RESOURCE_2, AiOrderType::COLLECT_RESOURCE_3
	                    }, lookbackTicks);
}

float AiHistory::attackFailureScore(unsigned int lookbackTicks) const {
	return failureScore({
			                    AiOrderType::ATTACK_ECON, AiOrderType::ATTACK_BUILDING, AiOrderType::ATTACK_ARMY
	                    }, lookbackTicks);
}

float AiHistory::defendFailureScore(unsigned int lookbackTicks) const {
	return failureScore({
			                    AiOrderType::DEFEND_ECON, AiOrderType::DEFEND_BUILDING, AiOrderType::DEFEND_ARMY
	                    }, lookbackTicks);
}

float AiHistory::attackActivityScore(unsigned int lookbackTicks) const {
	return recencyScore({
			                    AiOrderType::ATTACK_ECON, AiOrderType::ATTACK_BUILDING, AiOrderType::ATTACK_ARMY
	                    }, lookbackTicks);
}

float AiHistory::defendActivityScore(unsigned int lookbackTicks) const {
	return recencyScore({
			                    AiOrderType::DEFEND_ECON, AiOrderType::DEFEND_BUILDING, AiOrderType::DEFEND_ARMY
	                    }, lookbackTicks);
}
