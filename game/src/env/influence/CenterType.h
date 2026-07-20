#pragma once
enum class CenterType:char {
	ECON=0,
	BUILDING,
	ARMY,
	// TODO: BATTLE currently aliases the player's attack activity map; it is not a shared battle center.
	BATTLE
};
