#pragma once
#include "database/db_strcut.h"
#include <Urho3D/Container/Str.h>
#include <vector>


Urho3D::String stringFrom(const Urho3D::String& name, std::vector<db_cost*>* costs) {
	auto msg = name + "\n";
	for (db_cost* cost : *costs) {
		msg += cost->resourceName + " - " + Urho3D::String(cost->value) + "\n";
	}
	return msg;
}
