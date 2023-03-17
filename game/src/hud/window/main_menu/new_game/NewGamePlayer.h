#pragma once
#include <Urho3D/Container/Str.h>

struct NewGamePlayer {
	Urho3D::String name;
	int nation;
	int team;
	int color;
	int id;
	bool active;
};
