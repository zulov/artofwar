#pragma once
enum class UnitStateType :short
{
	GO = 0,//do celu
	STOP,
	CHARAGE,
	ATTACK,
	PATROL,
	DEAD,
	DEFEND,
	FOLLOW,//porusza sie
	COLLECT,
	MOVE,//TODO nie ma takiego orderu musi byæ rêcznie dodany?
	DISPOSE
};
