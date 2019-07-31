#include "ColorPaletteRepo.h"
#include "Game.h"
#include "objects/unit/state/StateManager.h"
#include <Urho3D/Resource/ResourceCache.h>


ColorPaletteRepo::ColorPaletteRepo() {
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
	for (int i = 0; i < SPECTRUM_RESOLUTION; ++i) {
		basicSpectrum[i] = Urho3D::Color(0, 0, i * (1.0 / SPECTRUM_RESOLUTION));
	}
	basicSpectrum[SPECTRUM_RESOLUTION] = basicSpectrum[SPECTRUM_RESOLUTION - 1];
}


ColorPaletteRepo::~ColorPaletteRepo() = default;

Urho3D::Material* ColorPaletteRepo::getColor(ColorPallet colorPallet, float value, float maxValue) {
	value = fixValue(value, maxValue);
	const int index = value / maxValue * PALLET_RESOLUTION;

	switch (colorPallet) {
	case ColorPallet::RED:
		return redPallet[index];
	}
	return nullptr;
}

float ColorPaletteRepo::fixValue(float value, float maxValue) {
	if (value > maxValue) {
		return maxValue;
	}
	if (value < 0) {
		return 0;
	}
	return value;
}

Urho3D::Color& ColorPaletteRepo::getColor(float value, float maxValue) {
	value = fixValue(value, maxValue);
	const int index = value / maxValue * SPECTRUM_RESOLUTION;
	return basicSpectrum[index];
}

Urho3D::Material* ColorPaletteRepo::getColor(UnitState state) {
	return statePallet[static_cast<char>(state)];
}

Urho3D::Material* ColorPaletteRepo::getLineMaterial() const {
	return lineMaterial;
}
