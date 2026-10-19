#pragma once

#include <algorithm>
#include <cmath>

struct ArmyUpkeepSettlement {
	float totalCost{};
	float freeAllowance{};
	float due{};
	float paid{};
	float shortfall{};
	float starvationFraction{};
};

inline ArmyUpkeepSettlement calculateArmyUpkeep(float totalCost, float freeAllowance, float availableFood,
		float starvationDamageRate) {
	const auto nonNegative = [](float value) { return std::isfinite(value) ? std::max(0.f, value) : 0.f; };

	ArmyUpkeepSettlement result;
	result.totalCost = nonNegative(totalCost);
	result.freeAllowance = nonNegative(freeAllowance);
	result.due = std::max(0.f, result.totalCost - result.freeAllowance);
	result.paid = std::min(result.due, nonNegative(availableFood));
	result.shortfall = result.due - result.paid;
	result.starvationFraction = result.due > 0.f
		? std::clamp(result.shortfall / result.due, 0.f, 1.f) * std::clamp(nonNegative(starvationDamageRate), 0.f, 1.f)
		: 0.f;
	return result;
}
