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
	MOVE,
	FOLLOW,//porusza sie
	COLLECT
};
