#pragma once
#include "objects/unit/ActionParameter.h"
#define stringify( name ) # name

struct Consts {
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

	inline static const unsigned char bitFlags[8] =
	{
		0x1,
		0x2,
		0x4,
		0x8,
		0x10,
		0x20,
		0x40,
		0x80
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

	inline static char* StaticStateNames[] = {
		stringify(ALIVE),
		stringify(DEAD),
		stringify(DISPOSE),
		stringify(FREE)
	};
};
