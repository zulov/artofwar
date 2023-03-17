#pragma once

enum class UnitState : char {
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
