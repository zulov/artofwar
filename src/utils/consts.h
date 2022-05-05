#pragma once
#include "objects/unit/ActionParameter.h"

struct Consts {
	inline static Urho3D::Vector2 circleCords[8] =
	{
		{0.f, 1.f},
		{0.71f, 0.71f},
		{1, 0},
		{-0.71f, -0.71f},
		{0.f, -1.f},
		{-0.71f, -0.71f},
		{-1.f, 0.f},
		{-0.71f, 0.71f}
	};

	inline constexpr static ActionParameter EMPTY_ACTION_PARAMETER = ActionParameter();

	inline const static std::vector<Physical*> EMPTY_PHYSICAL;

	inline const static Urho3D::Vector3 DIRS[4] = {
		Urho3D::Vector3::FORWARD, Urho3D::Vector3::BACK, Urho3D::Vector3::LEFT, Urho3D::Vector3::RIGHT
	};
};
