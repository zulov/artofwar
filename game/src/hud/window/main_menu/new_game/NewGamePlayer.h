#pragma once
#include <Urho3D/Container/Str.h>

struct NewGamePlayer {
	Urho3D::String name;
	char nation;
	char team;
	int color;
	char id;
	bool active;
};
