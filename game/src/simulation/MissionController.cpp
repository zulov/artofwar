#include "MissionController.h"

#include <algorithm>
#include <ranges>

#include "database/db_struct.h"
#include "objects/building/Building.h"
#include "objects/unit/Unit.h"
#include "player/Player.h"
#include "player/Possession.h"
#include "player/Resources.h"

MissionController::MissionController(const db_map* map) : map(map) {
	if (map) {
		goals.reserve(map->goals.size());
		for (const auto* goal : map->goals) {
			goals.push_back({goal});
		}
	}
}

std::vector<MissionGoalDeadlineSaveData> MissionController::saveDeadlines(unsigned char player) const {
	std::vector<MissionGoalDeadlineSaveData> result;
	for (size_t i = 0; i < goals.size(); ++i) {
		const auto& state = goals[i];
		if (state.goal->type == MissionGoalType::SCORE_LEAD_AT_TIME && state.deadlineEvaluated &&
			state.deadlinePlayer == player) {
			result.push_back({player, static_cast<unsigned short>(i), state.deadlineSuccess,
				state.deadlineScore, state.deadlineEnemyScore});
		}
	}
	return result;
}

void MissionController::restoreDeadlines(const std::vector<MissionGoalDeadlineSaveData>& deadlines,
		unsigned char player) {
	lastActivePlayer = player;
	for (const auto& saved : deadlines) {
		if (saved.player != player) continue;
		if (saved.goalId >= goals.size()) continue;
		auto& state = goals[saved.goalId];
		if (state.goal->type != MissionGoalType::SCORE_LEAD_AT_TIME) continue;
		state.deadlineEvaluated = true;
		state.deadlineSuccess = saved.success;
		state.deadlinePlayer = saved.player;
		state.deadlineScore = saved.playerScore;
		state.deadlineEnemyScore = saved.enemyScore;
		state.value = saved.playerScore;
		state.target = 1.f;
		state.complete = saved.success;
	}
}

void MissionController::update(Player* activePlayer, const std::vector<Player*>& players, unsigned totalTicks) {
	if (!activePlayer) return;
	if (lastActivePlayer != activePlayer->getId()) {
		lastActivePlayer = activePlayer->getId();
		result = MissionResult::RUNNING;
		for (auto& state : goals) {
			if (state.goal->type == MissionGoalType::SCORE_LEAD_AT_TIME) {
				state.deadlineEvaluated = false;
				state.deadlineSuccess = false;
			}
		}
	}
	if (result != MissionResult::RUNNING) return;

	for (auto& state : goals) {
		if (state.goal->type == MissionGoalType::SCORE_LEAD_AT_TIME) {
			if (!state.deadlineEvaluated && totalTicks >= state.goal->timeTicks) {
				auto* enemy = getTargetPlayer(state.goal->targetPlayer, *activePlayer, players);
				state.deadlineScore = static_cast<float>(activePlayer->getScore());
				state.deadlineEnemyScore = enemy ? static_cast<float>(enemy->getScore()) : 0.f;
				state.deadlineSuccess = enemy && state.deadlineScore > state.deadlineEnemyScore;
				state.deadlineEvaluated = true;
				state.deadlinePlayer = activePlayer->getId();
			}
			if (state.deadlineEvaluated && state.deadlinePlayer != activePlayer->getId()) {
				state.deadlineEvaluated = false;
				state.deadlineSuccess = false;
				state.deadlineScore = 0.f;
				state.deadlineEnemyScore = 0.f;
			}
			if (!state.deadlineEvaluated) {
				state.deadlineScore = static_cast<float>(activePlayer->getScore());
				if (auto* enemy = getTargetPlayer(state.goal->targetPlayer, *activePlayer, players)) {
					state.deadlineEnemyScore = static_cast<float>(enemy->getScore());
				}
			}
			state.value = state.deadlineEvaluated ? state.deadlineScore : static_cast<float>(activePlayer->getScore());
			state.target = state.deadlineEvaluated ? 1.f : 0.f;
			state.complete = state.deadlineEvaluated && state.deadlineSuccess;
		} else {
			state.complete = goalValue(*state.goal, *activePlayer, players, totalTicks, state.value, state.target);
		}
	}

	if (!hasLivingEntities(*activePlayer, true, true)) {
		result = MissionResult::DEFEAT;
		return;
	}
	if (!goals.empty() && std::ranges::all_of(goals, [](const auto& state) { return state.complete; })) {
		result = MissionResult::VICTORY;
	}
}

bool MissionController::goalValue(const db_mission_goal& goal, Player& player,
		const std::vector<Player*>& players, unsigned totalTicks, float& value, float& target) const {
	const auto* possession = player.getPossession();
	auto* resources = player.getResources();
	target = goal.targetValue;
	value = 0.f;

	switch (goal.type) {
	case MissionGoalType::ELIMINATE_PLAYER: {
		const auto* targetPlayer = getTargetPlayer(goal.targetPlayer, player, players);
		if (!targetPlayer) return false;
		value = hasLivingEntities(*targetPlayer, true, true) ? 1.f : 0.f;
		target = 1.f;
		return value == 0.f;
	}
	case MissionGoalType::ELIMINATE_TEAM: {
		const auto remaining = enemyEntityCount(player, players, true, true, goal.targetPlayer, true);
		value = static_cast<float>(remaining);
		target = 0.f;
		return goal.targetPlayer >= 0 && remaining == 0;
	}
	case MissionGoalType::ELIMINATE_ALL: {
		const auto remaining = enemyEntityCount(player, players, true, true, -1, true);
		value = static_cast<float>(remaining);
		target = 0.f;
		return remaining == 0;
	}
	case MissionGoalType::ELIMINATE_UNITS: {
		const auto remaining = enemyEntityCount(player, players, true, false, goal.targetPlayer);
		value = static_cast<float>(remaining);
		target = 0.f;
		return remaining == 0;
	}
	case MissionGoalType::ELIMINATE_BUILDINGS: {
		const auto remaining = enemyEntityCount(player, players, false, true, goal.targetPlayer);
		value = static_cast<float>(remaining);
		target = 0.f;
		return remaining == 0;
	}
	case MissionGoalType::SURVIVE_WITH_BUILDING_UNTIL:
		value = static_cast<float>(totalTicks);
		return totalTicks >= goal.timeTicks && std::ranges::any_of(possession->getBuildings(),
		[&goal](const auto* building) {
			return building->isReady() && (goal.targetId < 0 || building->getDbId() == goal.targetId);
		});
	case MissionGoalType::SURVIVE_UNTIL:
		value = static_cast<float>(totalTicks);
		return totalTicks >= goal.timeTicks;
	case MissionGoalType::KILL_UNITS:
		value = static_cast<float>(player.getUnitKills());
		return value >= target;
	case MissionGoalType::SCORE_LEAD_AT_TIME: {
		value = static_cast<float>(player.getScore());
		auto* enemy = getTargetPlayer(goal.targetPlayer, player, players);
		return totalTicks >= goal.timeTicks && enemy && value > static_cast<float>(enemy->getScore());
	}
	case MissionGoalType::RESOURCE_STOCK:
		if (goal.targetId < 0 || goal.targetId >= RESOURCES_SIZE) return false;
		value = resources->getValues()[goal.targetId];
		return value >= target;
	case MissionGoalType::RESOURCE_GATHERED:
		if (goal.targetId < 0 || goal.targetId >= RESOURCES_SIZE) return false;
		value = resources->getSumValues()[goal.targetId];
		return value >= target;
	case MissionGoalType::BUILDING_COUNT:
		value = static_cast<float>(std::ranges::count_if(possession->getBuildings(), [&](const auto* building) {
			return building->isReady() && (goal.targetId < 0 || building->getDbId() == goal.targetId);
		}));
		return value >= target;
	case MissionGoalType::SCORE_REACH:
		value = static_cast<float>(player.getScore());
		return value >= target;
	}
	return false;
}

bool MissionController::hasLivingEntities(const Player& player, bool units, bool buildings) const {
	const auto* possession = player.getPossession();
	return (units && std::ranges::any_of(possession->getUnits(), [](const auto* unit) { return unit->isAlive(); })) ||
		(buildings && std::ranges::any_of(possession->getBuildings(), [](const auto* building) { return building->isAlive(); }));
}

unsigned MissionController::enemyEntityCount(const Player& player, const std::vector<Player*>& players, bool units,
		bool buildings, short targetPlayer, bool targetTeam) const {
	unsigned count = 0;
	short targetTeamId = -1;
	if (targetTeam && targetPlayer >= 0) {
		const auto it = std::ranges::find_if(players, [targetPlayer](const auto* candidate) {
			return candidate && candidate->getId() == targetPlayer;
		});
		if (it == players.end()) return 0;
		targetTeamId = (*it)->getTeam();
	}
	for (const auto* candidate : players) {
		if (!candidate || candidate->getTeam() == player.getTeam()) continue;
		if (targetPlayer >= 0 && (targetTeam ? candidate->getTeam() != targetTeamId :
			candidate->getId() != targetPlayer)) continue;
		const auto* possession = candidate->getPossession();
		if (units) {
			count += static_cast<unsigned>(std::ranges::count_if(possession->getUnits(), [](const auto* unit) {
				return unit->isAlive();
			}));
		}
		if (buildings) {
			count += static_cast<unsigned>(std::ranges::count_if(possession->getBuildings(), [](const auto* building) {
				return building->isAlive();
			}));
		}
	}
	return count;
}

Player* MissionController::getTargetPlayer(short targetPlayer, const Player& player,
		const std::vector<Player*>& players) const {
	if (targetPlayer >= 0) {
		const auto it = std::ranges::find_if(players, [targetPlayer](const auto* candidate) {
			return candidate && candidate->getId() == targetPlayer;
		});
		return it == players.end() ? nullptr : *it;
	}
	const auto it = std::ranges::find_if(players, [&player](const auto* candidate) {
		return candidate && candidate->getTeam() != player.getTeam();
	});
	return it == players.end() ? nullptr : *it;
}
