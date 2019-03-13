#pragma once
#include "objects/unit/ActionParameter.h"
#define stringify( name ) # name

struct Consts
{
	inline static Urho3D::Vector2 circleCords[8] =
	{
		{0, 1},
		{0.71, 0.71},
		{1, 0},
		{-0.71, -0.71},
		{0, -1},
		{-0.71, -0.71},
		{-1, 0},
		{-0.71, 0.71}
	};
	inline const static ActionParameter EMPTY_ACTION_PARAMETER = ActionParameter::Builder().build();

	inline static char* UnitStateNames[] =
	{
		stringify(STOP),
		stringify(MOVE),
		stringify(GO_TO),
		stringify(DEAD),
		stringify(CHARGE),
		stringify(ATTACK),
		stringify(SHOT),
		stringify(DEFEND),
		stringify(FOLLOW),
		stringify(COLLECT),
		stringify(DISPOSE)
	};
};
