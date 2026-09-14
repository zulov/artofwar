#pragma once

enum class UnitState : unsigned char {
	STOP=0,
	MOVE,
	GO,
	FOLLOW,

	ATTACK,
	SHOT,
	CHARGE,
	DEFEND,
	COLLECT,

	DEAD,
	DISPOSE
	
};
