#pragma once
enum class QueueActionType : char {
	UNIT_CREATE=0,
	UNIT_LEVEL,
	UNIT_UPGRADE,//TODO dodac to kiedys
	BUILDING_LEVEL,
	BUILDING_CREATE,
};
