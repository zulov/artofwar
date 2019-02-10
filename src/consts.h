#pragma once

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
	inline const static ActionParameter EMPTY_INSTANCE = ActionParameter::Builder().build();
};
