#include "AiHistory.h"
#include <algorithm>
#include "Game.h"

namespace {
	constexpr unsigned int LOOKBACK_TICKS = 1200;
}

void AiHistory::addAction(AiActionType type, AiActionResult result, uint8_t chosenId) {
	actions[actionHead] = {Game::getFrameInfo()->getTotalTicks(), type, result, chosenId};
	// std::cout << magic_enum::enum_name(type) << " " << magic_enum::enum_name(result) << std::endl;
	actionHead = (actionHead + 1) % MAX_ENTRIES;
	actionCount = std::min(actionCount + 1, MAX_ENTRIES);
	scoresValid = false;
}

void AiHistory::addOrder(AiOrderType type, AiOrderResult result, uint8_t unitCount) {
	orders[orderHead] = {Game::getFrameInfo()->getTotalTicks(), type, result, unitCount};
	orderHead = (orderHead + 1) % MAX_ENTRIES;
	orderCount = std::min(orderCount + 1, MAX_ENTRIES);
	scoresValid = false;
}

const ActionHistoryEntry& AiHistory::getAction(int index) const {
	int actual = (actionHead - actionCount + index + MAX_ENTRIES) % MAX_ENTRIES;
	return actions[actual];
}

const OrderHistoryEntry& AiHistory::getOrder(int index) const {
	int actual = (orderHead - orderCount + index + MAX_ENTRIES) % MAX_ENTRIES;
	return orders[actual];
}

float AiHistory::recencyScore(AiActionType type) const {
	ensureScores();
	return actionSuccessScores[static_cast<size_t>(type)];
}

float AiHistory::recencyScore(std::initializer_list<AiActionType> types) const {
	ensureScores();
	float score = 0.f;
	for (auto type : types) { score += actionSuccessScores[static_cast<size_t>(type)]; }
	return score;
}

float AiHistory::recencyScore(std::initializer_list<AiOrderType> types) const {
	ensureScores();
	float score = 0.f;
	for (auto type : types) { score += orderSuccessScores[static_cast<size_t>(type)]; }
	return score;
}

float AiHistory::failureScore(std::initializer_list<AiActionType> types) const {
	ensureScores();
	float score = 0.f;
	for (auto type : types) { score += actionFailureScores[static_cast<size_t>(type)]; }
	return score;
}

float AiHistory::failureScore(std::initializer_list<AiOrderType> types) const {
	ensureScores();
	float score = 0.f;
	for (auto type : types) { score += orderFailureScores[static_cast<size_t>(type)]; }
	return score;
}

float AiHistory::recencyScore(AiOrderType type) const {
	ensureScores();
	return orderSuccessScores[static_cast<size_t>(type)];
}

float AiHistory::failureScore(AiActionType type) const {
	ensureScores();
	return actionFailureScores[static_cast<size_t>(type)];
}

void AiHistory::ensureScores() const {
	const unsigned int now = Game::getFrameInfo()->getTotalTicks();
	if (scoresValid && scoresTick == now) { return; }

	actionSuccessScores.fill(0.f);
	actionFailureScores.fill(0.f);
	orderSuccessScores.fill(0.f);
	orderFailureScores.fill(0.f);

	const unsigned int minTick = now > LOOKBACK_TICKS ? now - LOOKBACK_TICKS : 0;
	const float lookback = static_cast<float>(LOOKBACK_TICKS);

	for (int i = actionCount - 1; i >= 0; --i) {
		const auto& entry = getAction(i);
		if (entry.tick < minTick) { break; }
		const float score = 1.f - static_cast<float>(now - entry.tick) / lookback;
		const auto type = static_cast<size_t>(entry.actionType);
		if (entry.result == AiActionResult::SUCCESS) {
			actionSuccessScores[type] += score;
		} else {
			actionFailureScores[type] += score;
		}
	}

	for (int i = orderCount - 1; i >= 0; --i) {
		const auto& entry = getOrder(i);
		if (entry.tick < minTick) { break; }
		const float score = 1.f - static_cast<float>(now - entry.tick) / lookback;
		const auto type = static_cast<size_t>(entry.orderType);
		if (entry.result == AiOrderResult::SUCCESS) {
			orderSuccessScores[type] += score;
		} else {
			orderFailureScores[type] += score;
		}
	}

	scoresTick = now;
	scoresValid = true;
}

float AiHistory::buildingFailureScore() const {
	return failureScore({AiActionType::CREATE_BUILDING});
}

float AiHistory::unitFailureScore() const {
	return failureScore(AiActionType::CREATE_UNIT);
}

float AiHistory::collectFailureScore() const {
	return failureScore({AiOrderType::COLLECT_RESOURCE_0, AiOrderType::COLLECT_RESOURCE_1,
			                    AiOrderType::COLLECT_RESOURCE_2, AiOrderType::COLLECT_RESOURCE_3});
}

float AiHistory::attackFailureScore() const {
	return failureScore({AiOrderType::ATTACK_ECON, AiOrderType::ATTACK_BUILDING, AiOrderType::ATTACK_ARMY});
}

float AiHistory::defendFailureScore() const {
	return failureScore({AiOrderType::DEFEND_ECON, AiOrderType::DEFEND_BUILDING, AiOrderType::DEFEND_ARMY});
}

float AiHistory::attackActivityScore() const {
	return recencyScore({AiOrderType::ATTACK_ECON, AiOrderType::ATTACK_BUILDING, AiOrderType::ATTACK_ARMY});
}

float AiHistory::defendActivityScore() const {
	return recencyScore({AiOrderType::DEFEND_ECON, AiOrderType::DEFEND_BUILDING, AiOrderType::DEFEND_ARMY});
}
