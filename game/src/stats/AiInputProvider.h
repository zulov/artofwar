#pragma once
#include <span>
#include "database/db_strcut.h"
#include "player/ai/MetricDefinitions.h"

class Player;
struct db_unit_metric;
struct db_building_metric;

class AiInputProvider {
public:
	AiInputProvider() = default;
	AiInputProvider(const AiInputProvider&) = delete;

	std::span<const float> getResourceInput(Player* player, Player* enemy);

	std::span<const float> getUnitsInput(Player* player);
	std::span<const float> getBuildingsInput(Player* player);

	std::span<const float> getUnitsInputWithMetric(Player* player, const db_unit_metric* prop);
	std::span<const float> getBuildingsInputWithMetric(Player* player, const db_building_metric* prop, ParentBuildingType type);

	std::span<const float> getAttackOrDefenceInput(Player* player, Player* enemy);
	std::span<const float> getWhereAttack(Player* player, Player* enemy);
	std::span<const float> getWhereDefend(Player* player, Player* enemy);
	std::span<const float> getBuildingsTypeInput(Player* player, Player* enemy, ParentBuildingType type);
private:
	std::span<const float> basicWithSpan(Player* player, std::span<const float> span);
};
