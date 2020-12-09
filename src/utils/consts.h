#pragma once
#include "objects/queue/QueueManager.h"
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

	inline const static ActionParameter EMPTY_ACTION_PARAMETER = ActionParameter();

	inline const static std::vector<Physical*> EMPTY_PHYSICAL;

	inline static QueueManager EMPTY_QUEUE = QueueManager(0);

	inline const static Urho3D::Vector3 DIRS[4] = {
		Urho3D::Vector3::FORWARD, Urho3D::Vector3::BACK, Urho3D::Vector3::LEFT, Urho3D::Vector3::RIGHT
	};

};
