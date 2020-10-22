#include "ColorPaletteRepo.h"

#include <Urho3D/Graphics/Material.h>
#include <Urho3D/Resource/ResourceCache.h>
#include "Game.h"
#include "math/MathUtils.h"
#include "objects/CellState.h"
#include "objects/unit/state/StateManager.h"


ColorPaletteRepo::ColorPaletteRepo() {
	for (int i = 0; i < PALLET_RESOLUTION; ++i) {
		redPallet[i] = new Urho3D::Material(Game::getContext());
		redPallet[i]->SetShaderParameter("MatDiffColor", Urho3D::Color(i * (1.0f / PALLET_RESOLUTION), 0, 0));
		redPallet[i]->SetName("PaletteRed_" + Urho3D::String(i));
		Game::getCache()->AddManualResource(redPallet[i]);
	}
	redPallet[PALLET_RESOLUTION] = redPallet[PALLET_RESOLUTION - 1];

	for (int i = 0; i < magic_enum::enum_count<UnitState>(); ++i) {
		statePallet[i] = new Urho3D::Material(Game::getContext());
		statePallet[i]->SetShaderParameter("MatDiffColor",
		                                   Urho3D::Color(i * (1.0f / magic_enum::enum_count<UnitState>()), 0, 0));
		statePallet[i]->SetName("StatePallet_" + Urho3D::String(i));
		Game::getCache()->AddManualResource(statePallet[i]);
	}
	lineMaterial = Game::getCache()->GetResource<Urho3D::Material>("Materials/line.xml");
	for (int i = 0; i < SPECTRUM_RESOLUTION; ++i) {
		basicSpectrum[i] = Urho3D::Color(0.1, i * (1.0f / SPECTRUM_RESOLUTION), 0.1, 0.5);
	}
	basicSpectrum[SPECTRUM_RESOLUTION] = basicSpectrum[SPECTRUM_RESOLUTION - 1];
}

Urho3D::Material* ColorPaletteRepo::getColor(ColorPallet colorPallet, float value, float maxValue) {
	value = fixValue(value, maxValue);
	const int index = value / maxValue * PALLET_RESOLUTION;

	switch (colorPallet) {
	case ColorPallet::RED:
		return redPallet[index];
	}
	return nullptr;
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

std::tuple<bool, Urho3D::Color> ColorPaletteRepo::getInfoForGrid(CellState state) {
	switch (state) {
	case CellState::EMPTY: return {false, Urho3D::Color(0, 0, 0, 0)};
	case CellState::ATTACK: return {true, Urho3D::Color::RED};
	case CellState::COLLECT: return {true, Urho3D::Color::YELLOW};
	case CellState::NONE: return {false, Urho3D::Color(0, 0, 0, 0)};
	case CellState::RESOURCE: return {true, Urho3D::Color::GREEN};
	case CellState::BUILDING: return {true, Urho3D::Color::BLUE};
	case CellState::DEPLOY: return {true, Urho3D::Color::CYAN};
	default: ;
	}
}
