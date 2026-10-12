#pragma once
enum class ActionType : unsigned char {
	NONE=0,

	UNIT_CREATE,
	UNIT_LEVEL,

	BUILDING_CREATE,
	BUILDING_LEVEL,

	ORDER,
	FORMATION,

	RESOURCE,
	TECH_RESEARCH
};
