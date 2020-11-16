#pragma once
#include "database/db_strcut.h"
#include <Urho3D/Container/Str.h>
#include <vector>

Urho3D::String stringFrom(const Urho3D::String& name, const std::vector<db_cost*>& costs) {
	auto msg = name + "\n";
	for (db_cost* cost : costs) {		
		msg += Game::getDatabase()->getResource(cost->resource)->name + " - " + Urho3D::String(cost->value) + "\n";
	}
	return msg;
}
