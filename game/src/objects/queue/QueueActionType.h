#pragma once
enum class QueueActionType : unsigned char {
	UNIT_CREATE=0,
	UNIT_LEVEL,
	UNIT_UPGRADE,//TODO dodac to kiedys
	BUILDING_LEVEL,
	BUILDING_CREATE,
	RESOURCE_CREATE,
};
