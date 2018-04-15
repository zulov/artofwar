#include "ColorPeletteRepo.h"
#include "Game.h"
#include <Urho3D/Resource/ResourceCache.h>
#include <iostream>


ColorPeletteRepo::ColorPeletteRepo() {
	for (int i = 0; i < PALLET_RESOLUTION; ++i) {
		redPallet[i] = new Material(Game::get()->getContext());
		redPallet[i]->SetShaderParameter("MatDiffColor", Color(i * (1.0 / PALLET_RESOLUTION), 0, 0));
		redPallet[i]->SetName("PaletteRed_" + String(i));
		Game::get()->getCache()->AddManualResource(redPallet[i]);
	}
	redPallet[PALLET_RESOLUTION] = redPallet[PALLET_RESOLUTION - 1];

	for (int i = 0; i < STATE_SIZE; ++i) {
		statePallet[i] = new Material(Game::get()->getContext());
		statePallet[i]->SetShaderParameter("MatDiffColor", Color(i * (1.0 / STATE_SIZE), 0, 0));
		statePallet[i]->SetName("StatePallet_" + String(i));
		Game::get()->getCache()->AddManualResource(statePallet[i]);
	}
}


ColorPeletteRepo::~ColorPeletteRepo() = default;

Urho3D::Material* ColorPeletteRepo::getColor(ColorPallet colorPallet, float value, float maxValue) {
	if (value > maxValue) {
		value = maxValue;
	} else if (value < 0) {
		value = 0;
	}
	const int index = value / maxValue * PALLET_RESOLUTION;

	switch (colorPallet) {
	case ColorPallet::RED:
		return redPallet[index];
	}
	return nullptr;
}

Urho3D::Material* ColorPeletteRepo::getColor(UnitStateType state) {
	return statePallet[static_cast<char>(state)];
}
