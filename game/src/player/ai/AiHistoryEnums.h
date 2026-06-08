#pragma once
#include <cstdint>

#include "objects/resource/ResourceType.h"

enum class AiActionType : uint8_t {
	CREATE_WORKER,
	CREATE_UNIT,
	CREATE_BUILDING_OTHER,
	CREATE_BUILDING_DEFENCE,
	CREATE_BUILDING_RESOURCE,
	CREATE_BUILDING_TECH,
	CREATE_BUILDING_UNITS,
	UPGRADE_UNIT,
	UPGRADE_BUILDING,
	NONE
};

enum class AiActionResult : uint8_t {
	SUCCESS,
	NO_RESOURCES_SPECIFIC,
	NO_BUILDING_TO_DEPLOY,
	NO_POSITION_TO_BUILD,
	NO_BUILDINGS_OF_TYPE,
	NO_RESOURCE_BONUS,
	NO_UPGRADE_AVAILABLE,
	NONE
};

enum class AiOrderType : uint8_t {
	COLLECT_RESOURCE_0,
	COLLECT_RESOURCE_1,
	COLLECT_RESOURCE_2,
	COLLECT_RESOURCE_3,
	ATTACK_ECON,
	ATTACK_BUILDING,
	ATTACK_ARMY,
	DEFEND_ECON,
	DEFEND_BUILDING,
	DEFEND_ARMY,
	NONE
};

// The COLLECT_RESOURCE_<n> entries are addressed positionally as
// AiOrderType(COLLECT_RESOURCE_0 + resourceId) (see AiOrchestrator), so they must
// stay contiguous, start at 0, and have exactly one entry per ResourceType.
static_assert(static_cast<uint8_t>(AiOrderType::COLLECT_RESOURCE_0) == 0,
              "COLLECT_RESOURCE_0 must be the first AiOrderType (positional indexing by resourceId)");
static_assert(static_cast<uint8_t>(AiOrderType::COLLECT_RESOURCE_3)
              == static_cast<uint8_t>(AiOrderType::COLLECT_RESOURCE_0) + (RESOURCES_SIZE - 1),
              "There must be exactly one COLLECT_RESOURCE_<n> entry per ResourceType, contiguous from 0");

enum class AiOrderResult : uint8_t {
	SUCCESS,
	NO_RESOURCE_IN_RANGE,
	NO_CENTER_POSITION,
	NONE
};

