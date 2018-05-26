#pragma once
#define STATE_SIZE 11

enum class UnitStateType : char
{
	GO = 0,
	//do celu
	STOP,
	CHARAGE,
	ATTACK,
	DEAD,
	DEFEND,
	FOLLOW,
	//porusza sie
	COLLECT,
	MOVE,
	//TODO nie ma takiego orderu musi byæ rêcznie dodany?
	DISPOSE,
	SHOT
};
