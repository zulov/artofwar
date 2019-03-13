#pragma once
#define STATE_SIZE 11

enum class UnitState : char
{
	STOP=0,
	MOVE,
	GO_TO,
	DEAD,
	CHARGE,
	ATTACK,
	SHOT,
	DEFEND,
	FOLLOW,
	COLLECT,
	DISPOSE
};

