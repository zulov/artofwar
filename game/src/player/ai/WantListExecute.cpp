#include "WantList.h"

#include <algorithm>

#include "database/db_struct.h"
#include "Game.h"
#include "utils/PrintUtils.h"

// The execution half of WantList. Kept in its own translation unit because it
// depends on Player/Resources (and their heavy transitive headers), whereas the
// scheduler half (WantList.cpp) is dependency-light and unit-testable on its own.

namespace {
	constexpr unsigned int ECON_DEBUG_SECONDS = 20;

	bool hasEnough(std::span<const float> resourceValues, const db_with_cost* cost) {
		for (int i = 0; i < RESOURCES_SIZE; ++i) {
			if (resourceValues[i] < cost->values[i]) {
				return false;
			}
		}
		return true;
	}

	void subtractCost(std::span<float> resourceValues, const db_with_cost* cost) {
		for (int i = 0; i < RESOURCES_SIZE; ++i) {
			resourceValues[i] -= static_cast<float>(cost->values[i]);
		}
	}

	void subtractSoftReserve(std::span<float> resourceValues, const db_with_cost* cost, unsigned char reserveTicks) {
		const float reserveFactor = static_cast<float>(reserveTicks) / 100.f;
		for (int i = 0; i < RESOURCES_SIZE; ++i) {
			const float reserveAmount = std::min(resourceValues[i], static_cast<float>(cost->values[i]) * reserveFactor);
			resourceValues[i] -= reserveAmount;
		}
	}
}

void WantList::addLacking(const db_with_cost* cost, std::span<const float> resourceValues, LackingResult& lacking) {
	if (!cost) { return; }
	for (int i = 0; i < RESOURCES_SIZE; ++i) {
		float deficit = static_cast<float>(cost->values[i]) - resourceValues[i];
		if (deficit > 0.f) {
			lacking.perResource[i] += deficit;
			lacking.totalSum += deficit;
		}
	}
}

WantList::LackingResult WantList::execute(std::span<const float> resourceValues, IWantExecutor& executor) {
	LackingResult lacking{};
	lacking.reset();

	boostOrDecay();
	sortByPriority();

	std::array<float, RESOURCES_SIZE> effectiveBudget{};
	std::ranges::copy(resourceValues, effectiveBudget.begin());

	for (auto& item : items) {
		const auto* cost = executor.cost(item);
		if (!cost) {
			item.reserveTicks = 0;
			continue;
		}

		for (unsigned char i = 0; i < item.count; ++i) {
			if (!hasEnough(effectiveBudget, cost)) {
				executor.onNotEnoughResources(item);
				addLacking(cost, effectiveBudget, lacking);
				if (item.reserveTicks < SOFT_RESERVE_MAX_PERCENT) {
					item.reserveTicks += SOFT_RESERVE_STEP_PERCENT;
				}
				subtractSoftReserve(effectiveBudget, cost, item.reserveTicks);
				break;
			}
			if (!executor.execute(item)) {
				item.reserveTicks = 0;
				break;
			}
			subtractCost(effectiveBudget, cost);
			item.reserveTicks = 0;
		}
	}

	dropDead();
	if (Game::getFrameInfo()->getSeconds() <= ECON_DEBUG_SECONDS) {
		PRINT("[ECON_LACKING]",
		      "sec", Game::getFrameInfo()->getSeconds(),
		      "lacking", lacking.perResource[0], lacking.perResource[1], lacking.perResource[2], lacking.perResource[3],
		      "total", lacking.totalSum);
	}
	return lacking;
}
