#pragma once
#include <span>
#include "database/db_strcut.h"
#include "player/ai/MetricDefinitions.h"

class Player;
struct db_unit_metric;
struct db_building_metric;
struct db_basic_metric;

class AiInputProvider {
public:
	AiInputProvider() = default;
	AiInputProvider(const AiInputProvider&) = delete;

	std::span<const float> getResourceInput(Player* player, Player* enemy);
	std::span<const float> getUnitsInput(Player* player);
	std::span<const float> getBuildingsInput(Player* player);

	std::span<const float> getUnitsInputWithMetric(Player* player, const db_unit_metric* prop);
	std::span<const float> getBuildingsInputWithMetric(Player* player, const db_building_metric* prop,
	                                                   ParentBuildingType type);

	std::span<const float> getAttackOrDefenceInput(Player* player, Player* enemy);
	std::span<const float> getWhereAttack(Player* player, Player* enemy);
	std::span<const float> getWhereDefend(Player* player, Player* enemy);
	std::span<const float> getBuildingsTypeInput(Player* player, Player* enemy, ParentBuildingType type);

private:
	template <std::size_t N>
	std::span<float> writeBasic(std::array<float, N>& output, Player* player);

	template <std::size_t N>
	std::span<const float> writeBasicWith(std::array<float, N>& output, Player* player, std::span<const float> second);
	//TODO mem, zastapic to jedna pzrestrzenia na dane
	std::array<float, BASIC_SIZE + std::max()> input;
	// std::array<float, BASIC_SIZE + RESOURCE_AI_SIZE> resourceIdInput;
	// std::array<float, BASIC_SIZE + UNIT_SIZE> unitsInput;
	// std::array<float, BASIC_SIZE + BUILDING_SIZE> buildingsInput;
	//
	// std::array<float, ATTACK_OR_DEFENCE_SIZE> attackOrDefenceInput;
	// std::array<float, BASIC_SIZE + WHERE_ATTACK_SIZE> whereAttackInput;
	// std::array<float, BASIC_SIZE + WHERE_DEFEND_SIZE> whereDefendInput;
	//
	// std::array<float, BASIC_SIZE + UNIT_TYPES_SIZE> unitsWithMetric;
	//
	// std::array<float, BASIC_SIZE + BUILDING_OTHER_SIZE> buildingsOtherInput;
	// std::array<float, BASIC_SIZE + BUILDING_DEF_SIZE> buildingsDefenceInput;
	// std::array<float, BASIC_SIZE + BUILDING_RES_BONUS_SIZE> buildingsResInput;
	// std::array<float, BASIC_SIZE + BUILDING_TECH_SIZE> buildingsTechInput;
	// std::array<float, BASIC_SIZE + BUILDING_UNITS_SIZE> buildingsUnitsInput;
	//
	// std::array<float, BASIC_SIZE + BUILDING_TYPES_SIZE> buildingsWhereInput;
	// std::array<float, BASIC_SIZE + BUILDING_RES_SIZE> buildingsResWhereInput;
};
