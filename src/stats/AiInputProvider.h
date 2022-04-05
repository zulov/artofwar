#pragma once
#include <span>
#include "database/db_strcut.h"
#include "player/ai/AiMetric.h"

class Player;
struct db_unit_metric;
struct db_building_metric;
struct db_basic_metric;

class AiInputProvider {
public:
	AiInputProvider();
	AiInputProvider(const AiInputProvider&) = delete;

	std::span<float> getResourceInput(char playerId) const;
	std::span<float> getUnitsInput(char playerId) const;
	std::span<float> getBuildingsInput(char playerId) const;

	std::span<float> getUnitsInputWithMetric(char playerId, const db_unit_metric* prop) const;
	std::span<float> getBuildingsInputWithMetric(char playerId, const db_building_metric* prop, ParentBuildingType type) const;

	std::span<float> getAttackOrDefenceInput(char playerId) const;
	std::span<float> getWhereAttack(char playerId) const;
	std::span<float> getWhereDefend(char playerId) const;
	std::span<float> getBuildingInputSpan(ParentBuildingType type) const;
	std::span<float> getBuildingsTypeInput(char playerId, ParentBuildingType type) const;
private:
	std::span<float> getBasicInput(std::span<float> dest, Player* player) const;
	std::span<float> combineWithBasic(const std::span<float> output, const std::span<const float> toJoin,
	                                  Player* player) const;
	std::span<float> combineWithBasic(const std::span<float> output, const std::vector<float>& toJoin,
	                                  Player* player) const;

	float resourceIdInput[BASIC_SIZE + RESOURCE_AI_SIZE];
	float unitsInput[BASIC_SIZE + UNIT_SIZE];
	float buildingsInput[BASIC_SIZE + BUILDING_SIZE];

	float attackOfDefenceInput[ATTACK_OR_DEFENCE_SIZE];
	float whereAttackInput[BASIC_SIZE + WHERE_ATTACK_SIZE];
	float whereDefendInput[BASIC_SIZE + WHERE_DEFEND_SIZE];

	float unitsWithMetric[BASIC_SIZE + UNIT_TYPES_SIZE];

	float buildingsOtherInput[BASIC_SIZE + BUILDING_OTHER_SIZE];
	float buildingsDefenceInput[BASIC_SIZE + BUILDING_DEF_SIZE];
	float buildingsResInput[BASIC_SIZE + BUILDING_RES_SIZE];
	float buildingsTechInput[BASIC_SIZE + BUILDING_TECH_SIZE];
	float buildingsUnitsInput[BASIC_SIZE + BUILDING_UNITS_SIZE];

	float buildingsWhereInput[BASIC_SIZE + BUILDING_TYPES_SIZE];
	float buildingsResWhereInput[BASIC_SIZE + BUILDING_RES_SIZE];

	std::span<float> resourceIdInputSpan = std::span(resourceIdInput);
	std::span<float> unitsInputSpan = std::span(unitsInput);
	std::span<float> buildingsInputSpan = std::span(buildingsInput);

	std::span<float> attackOfDefenceInputSpan = std::span(attackOfDefenceInput);
	std::span<float> whereAttackInputSpan = std::span(whereAttackInput);
	std::span<float> whereDefendInputSpan = std::span(whereDefendInput);

	std::span<float> unitsWithMetricUnitSpan = std::span(unitsWithMetric);

	std::span<float> buildingsWhereInputSpan = std::span(buildingsWhereInput);
	std::span<float> buildingsResWhereInputSpan = std::span(buildingsResWhereInput);

	std::span<float> buildingsOtherInputSpan = std::span(buildingsOtherInput);
	std::span<float> buildingsDefenceInputSpan = std::span(buildingsDefenceInput);
	std::span<float> buildingsResInputSpan = std::span(buildingsResInput);
	std::span<float> buildingsTechInputSpan = std::span(buildingsTechInput);
	std::span<float> buildingsUnitsInputSpan = std::span(buildingsUnitsInput);
};
