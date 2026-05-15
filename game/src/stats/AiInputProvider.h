#pragma once
#include <span>
#include "database/db_struct.h"

class Player;
struct db_unit_metric;
struct db_building_metric;

class AiInputProvider {
public:
	AiInputProvider() = default;
	AiInputProvider(const AiInputProvider&) = delete;

	std::span<const float> forResource(Player* player, Player* enemy);

	std::span<const float> forUnits(Player* player);
	std::span<const float> forBuildings(Player* player);

	std::span<const float> forUnitPlacement(Player* player, const db_unit_metric* prop);
	std::span<const float> forBuildingPlacement(Player* player, const db_building_metric* prop, ParentBuildingType type);

	std::span<const float> forAttackOrDefence(Player* player, Player* enemy);
	std::span<const float> forWhereAttack(Player* player, Player* enemy);
	std::span<const float> forWhereDefend(Player* player, Player* enemy);
	std::span<const float> forBuildingType(Player* player, ParentBuildingType type);
private:
	std::span<const float> basicWithSpan(Player* player, std::span<const float> span);
	std::span<const float> basicWithSpanSelective(Player* player, Player* enemy, std::span<const unsigned char> idxs);
};
