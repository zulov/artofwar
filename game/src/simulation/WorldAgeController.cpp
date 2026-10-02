#include "WorldAgeController.h"

#include <algorithm>
#include <cassert>
#include <charconv>
#include <string_view>

#include "database/db_world_age_struct.h"
#include "database/db_other_struct.h"
#include "database/db_struct.h"
#include "player/Player.h"
#include "player/Possession.h"
#include "scene/load/RuntimeSaveData.h"
#include "scene/load/dbload_container.h"

WorldAgeController::WorldAgeController(const db_world_age_catalog* catalog, const db_map* map)
	: catalog(catalog), ageIds(map ? &map->ageIds : nullptr) {
	assert(catalog != nullptr && ageIds != nullptr);
	reset();
}

void WorldAgeController::reset(unsigned ageStartedTick) {
	currentAgeId = 0;
	this->ageStartedTick = ageStartedTick;
	reachedAgeIds = {currentAgeId};
}

bool WorldAgeController::restore(const dbload_container& data) {
	if (!data.worldAgeState) {
		reset(data.config ? data.config->frame.totalTicks : 0);
		return true;
	}

	const auto& state = *data.worldAgeState;
	if (!catalog->getAge(state.currentAge) ||
		std::ranges::find(*ageIds, state.currentAge) == ageIds->end() ||
		(state.ageStartedTick > (data.config ? data.config->frame.totalTicks : 0))) {
		return false;
	}

	std::vector<unsigned short> restoredReached;
	std::string_view serializedHistory(state.history);
	while (!serializedHistory.empty()) {
		const auto comma = serializedHistory.find(',');
		const auto token = serializedHistory.substr(0, comma);
		unsigned short ageId{};
		const auto [end, error] = std::from_chars(token.data(), token.data() + token.size(), ageId);
		if (error != std::errc{} || end != token.data() + token.size()) {
			return false;
		}
		const auto* age = catalog->getAge(ageId);
		const auto* previous = restoredReached.empty() ? nullptr : catalog->getAge(restoredReached.back());
		if (!age || std::ranges::find(*ageIds, ageId) == ageIds->end() ||
			std::ranges::find(restoredReached, ageId) != restoredReached.end() ||
			(restoredReached.empty() ? ageId != 0 || age->stage != 0 :
				!previous || age->stage != previous->stage + 1 ||
				std::ranges::find(previous->nextAgeIds, ageId) == previous->nextAgeIds.end())) {
			return false;
		}
		restoredReached.push_back(ageId);
		if (comma == std::string_view::npos) {
			serializedHistory = {};
		} else {
			serializedHistory.remove_prefix(comma + 1);
		}
	}
	if (restoredReached.empty()) {
		if (state.currentAge != 0) {
			return false;
		}
		restoredReached.push_back(0);
	} else if (restoredReached.back() != state.currentAge) {
		return false;
	}

	currentAgeId = state.currentAge;
	ageStartedTick = state.ageStartedTick;
	reachedAgeIds = std::move(restoredReached);
	return true;
}

void WorldAgeController::update(const std::vector<Player*>& players, unsigned totalTicks) {
	const auto* current = catalog->getAge(currentAgeId);
	if (!current || players.empty()) {
		return;
	}

	for (const auto ageId : *ageIds) {
		const auto* age = catalog->getAge(ageId);
		if (age && isNextAge(*current, ageId) && ageMet(players, *age)) {
			advance(ageId, totalTicks);
			return;
		}
	}

	if (totalTicks - ageStartedTick >= AGE_TIMEOUT_TICKS) {
		const auto timeoutAgeId = selectTimeoutAge(players, *current);
		if (timeoutAgeId != currentAgeId) {
			advance(timeoutAgeId, totalTicks);
		}
	}
}

const db_world_age* WorldAgeController::getAge(unsigned short ageId) const {
	return catalog->getAge(ageId);
}

std::vector<WorldAgeProgress> WorldAgeController::getNextAgeProgress(const std::vector<Player*>& players) const {
	std::vector<WorldAgeProgress> result;
	const auto* current = catalog->getAge(currentAgeId);
	if (!current) {
		return result;
	}

	for (const auto ageId : *ageIds) {
		const auto* age = catalog->getAge(ageId);
		if (!age || !isNextAge(*current, ageId)) {
			continue;
		}

		const auto progress = players.empty() ? 0.f : ageProgress(players, *age);
		result.push_back({ageId, std::clamp(progress, 0.f, 1.f)});
	}
	return result;
}

float WorldAgeController::getTimeoutProgress(unsigned totalTicks) const {
	if (totalTicks <= ageStartedTick) {
		return 0.f;
	}
	return std::min(1.f, static_cast<float>(totalTicks - ageStartedTick) / AGE_TIMEOUT_TICKS);
}

bool WorldAgeController::hasReachedAge(unsigned short ageId) const {
	return std::ranges::find(reachedAgeIds, ageId) != reachedAgeIds.end();
}

bool WorldAgeController::isLevelAvailable(unsigned char ageStage) const {
	const auto* current = catalog->getAge(currentAgeId);
	return current && current->stage >= ageStage;
}

WorldAgeStateSaveData WorldAgeController::saveState() const {
	std::string serializedHistory;
	for (const auto ageId : reachedAgeIds) {
		if (!serializedHistory.empty()) {
			serializedHistory.push_back(',');
		}
		serializedHistory += std::to_string(ageId);
	}
	return {currentAgeId, ageStartedTick, std::move(serializedHistory)};
}

float WorldAgeController::getMetric(const Player& player, const db_world_age_condition& condition) const {
	switch (condition.metric) {
	case WorldAgeMetric::WORKER_COUNT:
		return static_cast<float>(player.getWorkersNumber());
	case WorldAgeMetric::ARMY_COUNT:
		return static_cast<float>(player.getPossession()->getArmyNumber());
	}
	assert(false && "world age condition has an unknown metric");
	return 0.f;
}

bool WorldAgeController::ageMet(const std::vector<Player*>& players, const db_world_age& age) const {
	float total = 0.f;
	for (const auto* condition : age.conditions) {
		for (const auto* player : players) {
			total += getMetric(*player, *condition);
		}
		if (total / static_cast<float>(players.size()) < condition->target) {
			return false;
		}
		total = 0.f;
	}
	return true;
}

float WorldAgeController::ageProgress(const std::vector<Player*>& players, const db_world_age& age) const {
	if (age.conditions.empty()) {
		return 0.f;
	}

	float progress = 1.f;
	for (const auto* condition : age.conditions) {
		float total = 0.f;
		for (const auto* player : players) {
			total += getMetric(*player, *condition);
		}
		progress = std::min(progress, total / static_cast<float>(players.size()) / condition->target);
	}
	return progress;
}

unsigned short WorldAgeController::selectTimeoutAge(const std::vector<Player*>& players,
		const db_world_age& current) const {
	const db_world_age* best = nullptr;
	float bestProgress = -1.f;
	for (const auto ageId : *ageIds) {
		const auto* age = catalog->getAge(ageId);
		if (!age || !isNextAge(current, ageId)) {
			continue;
		}
		const auto progress = ageProgress(players, *age);
		if (!best || progress > bestProgress) {
			best = age;
			bestProgress = progress;
		}
	}
	return best ? best->id : current.id;
}

bool WorldAgeController::isNextAge(const db_world_age& current, unsigned short ageId) const {
	return std::ranges::find(current.nextAgeIds, ageId) != current.nextAgeIds.end();
}

void WorldAgeController::advance(unsigned short ageId, unsigned totalTicks) {
	currentAgeId = ageId;
	ageStartedTick = totalTicks;
	reachedAgeIds.push_back(currentAgeId);
}
