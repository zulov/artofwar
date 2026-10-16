#pragma once
#include "Game.h"
#include "database/db_struct.h"
#include <Urho3D/Container/Str.h>
#include <Urho3D/Resource/Localization.h>

inline Urho3D::String stringFrom(const Urho3D::String& name, const db_with_cost* costs) {
	auto msg = name;
	// TODO: Replace resource names with dedicated font glyphs to display resource icons inline with cost text.
	const char* resourceKeys[] = {"resource_food", "resource_wood", "resource_stone", "resource_gold"};
	bool hasCost = false;
	for (int i = 0; i < costs->values.size(); ++i) {
		if (const auto val = costs->values[i]; val > 0) {
			if (hasCost) {
				msg += ", ";
			} else {
				msg += "\n";
			}
			msg += Urho3D::String(val) + " " + Game::getLocalization()->Get(resourceKeys[i]);
			hasCost = true;
		}
	}
	return msg;
}
