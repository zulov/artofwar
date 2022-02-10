#pragma once
#include <span>
#include "database/db_strcut.h"
#include "player/ai/AiMetric.h"

constexpr char BASIC_SIZE = std::size(METRIC_DEFINITIONS.aiBasicMetric);
constexpr char UNIT_SIZE = std::size(METRIC_DEFINITIONS.aiUnitMetric);
constexpr char BUILDING_SIZE = std::size(METRIC_DEFINITIONS.aiBuildingMetric);
constexpr char SMALL_UNIT_SIZE = std::size(METRIC_DEFINITIONS.aiSmallUnitMetric);
constexpr char SMALL_BUILDING_SIZE = std::size(METRIC_DEFINITIONS.aiSmallBuildingMetric);

constexpr char RESOURCE_AI_SIZE = std::size(METRIC_DEFINITIONS.aiResourceMetric);

constexpr char ATTACK_OR_DEFENCE_SIZE = std::size(METRIC_DEFINITIONS.aiAttackOrDefence);
constexpr char WHERE_ATTACK_SIZE = std::size(METRIC_DEFINITIONS.aiWhereAttack);
constexpr char WHERE_DEFEND_SIZE = std::size(METRIC_DEFINITIONS.aiWhereDefend);

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
	std::span<float> getBuildingsInputWithMetric(char playerId, const db_building_metric* prop) const;

	std::span<float> getAttackOrDefenceInput(char playerId) const;
	std::span<float> getWhereAttack(char playerId) const;
	std::span<float> getWhereDefend(char playerId) const;
private:
	std::span<float> getBasicInput(std::span<float> dest, Player* player) const;
	std::span<float> combineWithBasic(const std::span<float> output, const std::span<float> toJoin,
	                                  Player* player) const;
	std::span<float> combineWithBasic(const std::span<float> output, const std::vector<float>& toJoin,
	                                  Player* player) const;

	float resourceIdInput[BASIC_SIZE + RESOURCE_AI_SIZE];
	float unitsInput[BASIC_SIZE + UNIT_SIZE];
	float buildingsInput[BASIC_SIZE + BUILDING_SIZE];

	float attackOfDefenceInput[ATTACK_OR_DEFENCE_SIZE];
	float whereAttackInput[BASIC_SIZE + WHERE_ATTACK_SIZE];
	float whereDefendInput[BASIC_SIZE + WHERE_DEFEND_SIZE];

	float unitsWithMetric[BASIC_SIZE + SMALL_UNIT_SIZE];
	float buildingsWithMetric[BASIC_SIZE + SMALL_BUILDING_SIZE];

	std::span<float> resourceIdInputSpan = std::span(resourceIdInput);
	std::span<float> unitsInputSpan = std::span(unitsInput);
	std::span<float> buildingsInputSpan = std::span(buildingsInput);

	std::span<float> attackOfDefenceInputSpan = std::span(attackOfDefenceInput);
	std::span<float> whereAttackInputSpan = std::span(whereAttackInput);
	std::span<float> whereDefendInputSpan = std::span(whereDefendInput);

	std::span<float> unitsWithMetricUnitSpan = std::span(unitsWithMetric);
	std::span<float> basicWithMetricUnitSpan = std::span(buildingsWithMetric);
};
