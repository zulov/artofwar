#pragma once
#include "objects/unit/ActionParameter.h"

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

};
