#pragma once
#define STATE_SIZE 11

enum class UnitState : char
{
	GO = 0,
	//do celu
	STOP,
	CHARAGE,
	ATTACK,
	DEAD,
	DEFEND,
	FOLLOW,
	COLLECT,
	MOVE,
	//porusza sie
	//TODO nie ma takiego orderu musi byæ rêcznie dodany?
	DISPOSE,
	SHOT
};
