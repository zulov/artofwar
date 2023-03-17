#pragma once
#include "database/db_strcut.h"
#include <Urho3D/Container/Str.h>

Urho3D::String stringFrom(const Urho3D::String& name, const db_cost* costs) {
	auto msg = name + "\n";
	for (int i = 0; i < costs->values.size(); ++i) {
		if (const auto val = costs->values[i]; val > 0) {
			msg += Game::getDatabase()->getResource(i)->name + " - " + Urho3D::String(val) + "\n";
		}
	}
	return msg;
}
