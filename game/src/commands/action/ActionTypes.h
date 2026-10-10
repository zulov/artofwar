#pragma once
enum class BuildingActionType : unsigned char {
	UNIT_CREATE = 0,
	UNIT_LEVEL,
	UNIT_UPGRADE,
	TECH_RESEARCH,
};

enum class GeneralActionType : unsigned char { BUILDING_LEVEL = 0 };

enum class ResourceActionType : unsigned char { COLLECT = 0, CANCEL };
