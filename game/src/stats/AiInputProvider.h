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
	AiInputProvider();
	AiInputProvider(const AiInputProvider&) = delete;

	std::span<const float> getResourceInput(char playerId);
	std::span<const float> getUnitsInput(char playerId);
	std::span<const float> getBuildingsInput(char playerId);

	std::span<const float> getUnitsInputWithMetric(char playerId, const db_unit_metric* prop);
	std::span<const float> getBuildingsInputWithMetric(char playerId, const db_building_metric* prop,
	                                                   ParentBuildingType type);

	std::span<const float> getAttackOrDefenceInput(char playerId);
	std::span<const float> getWhereAttack(char playerId);
	std::span<const float> getWhereDefend(char playerId);
	std::span<const float> getBuildingsTypeInput(char playerId, ParentBuildingType type);

private:
	template <std::size_t N>
	std::span<const float> combineWithBasic(std::array<float, N>& output, std::span<const float> toJoin,
	                                        Player* player);

	std::array<float, BASIC_SIZE + RESOURCE_AI_SIZE> resourceIdInputArray;
	std::array<float, BASIC_SIZE + UNIT_SIZE> unitsInput;
	std::array<float, BASIC_SIZE + BUILDING_SIZE> buildingsInput;

	std::array<float, ATTACK_OR_DEFENCE_SIZE> attackOrDefenceInput;
	std::array<float, BASIC_SIZE + WHERE_ATTACK_SIZE> whereAttackInput;
	std::array<float, BASIC_SIZE + WHERE_DEFEND_SIZE> whereDefendInput;

	std::array<float, BASIC_SIZE + UNIT_TYPES_SIZE> unitsWithMetric;

	std::array<float, BASIC_SIZE + BUILDING_OTHER_SIZE> buildingsOtherInput;
	std::array<float, BASIC_SIZE + BUILDING_DEF_SIZE> buildingsDefenceInput;
	std::array<float, BASIC_SIZE + BUILDING_RES_BONUS_SIZE> buildingsResInput;
	std::array<float, BASIC_SIZE + BUILDING_TECH_SIZE> buildingsTechInput;
	std::array<float, BASIC_SIZE + BUILDING_UNITS_SIZE> buildingsUnitsInput;

	std::array<float, BASIC_SIZE + BUILDING_TYPES_SIZE> buildingsWhereInput;
	std::array<float, BASIC_SIZE + BUILDING_RES_SIZE> buildingsResWhereInput;
};
