#pragma once
enum class MenuAction : char
{
	NONE=0,
	
	UNIT_CREATE,
	UNIT_LEVEL,
	UNIT_UPGRADE,

	BUILDING,
	BUILDING_LEVEL,
	
	ORDER,
	FORMATION,
	
	RESOURCE
};
