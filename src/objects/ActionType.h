#pragma once
enum class ActionType : char
{
	NONE=0,
	
	UNIT_CREATE,
	UNIT_LEVEL,
	UNIT_UPGRADE,

	BUILDING_CREATE,
	BUILDING_LEVEL,
	
	ORDER,
	FORMATION,
	
	RESOURCE
};