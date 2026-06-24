#pragma once
enum class CenterType:char {
	ECON=0,
	BUILDING,
	ARMY,
	// TODO: BETTLE currently aliases the player's attackSpeed map; it is not a shared battle center.
	BETTLE
};
