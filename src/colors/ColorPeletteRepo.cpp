#include "ColorPeletteRepo.h"
#include "Game.h"
#include <Urho3D/Resource/ResourceCache.h>
#include <iostream>
#include "objects/unit/state/StateManager.h"


ColorPeletteRepo::ColorPeletteRepo() {
	for (int i = 0; i < PALLET_RESOLUTION; ++i) {
		redPallet[i] = new Urho3D::Material(Game::getContext());
		redPallet[i]->SetShaderParameter("MatDiffColor", Urho3D::Color(i * (1.0 / PALLET_RESOLUTION), 0, 0));
		redPallet[i]->SetName("PaletteRed_" + Urho3D::String(i));
		Game::getCache()->AddManualResource(redPallet[i]);
	}
	redPallet[PALLET_RESOLUTION] = redPallet[PALLET_RESOLUTION - 1];

	for (int i = 0; i < STATE_SIZE; ++i) {
		statePallet[i] = new Urho3D::Material(Game::getContext());
		statePallet[i]->SetShaderParameter("MatDiffColor", Urho3D::Color(i * (1.0 / STATE_SIZE), 0, 0));
		statePallet[i]->SetName("StatePallet_" + Urho3D::String(i));
		Game::getCache()->AddManualResource(statePallet[i]);
	}
	lineMaterial = Game::getCache()->GetResource<Urho3D::Material>("Materials/line.xml");
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

Urho3D::Material* ColorPeletteRepo::getColor(UnitState state) {
	return statePallet[static_cast<char>(state)];
}

Urho3D::Material* ColorPeletteRepo::getLineMaterial() const {
	return lineMaterial;
}
