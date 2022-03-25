#include "ColorPaletteRepo.h"

#include <Urho3D/Graphics/Material.h>
#include <Urho3D/Resource/ResourceCache.h>
#include "ColorPallet.h"
#include "Game.h"
#include "database/db_strcut.h"
#include "math/MathUtils.h"
#include "objects/CellState.h"
#include "player/Player.h"
#include "player/PlayersManager.h"
#include "simulation/SimGlobals.h"
#include "env/Environment.h"
#include "utils/OtherUtils.h"

ColorPaletteRepo::ColorPaletteRepo() {
	if (!SIM_GLOBALS.HEADLESS) {
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
		influenceMaterial = Game::getCache()->GetResource<Urho3D::Material>("Materials/influence.xml");
		for (int i = 0; i < SPECTRUM_RESOLUTION; ++i) {
			basicSpectrum[i] = Urho3D::Color(0.1, i * (1.0f / SPECTRUM_RESOLUTION), 0.1, 0.5);
		}
		basicSpectrum[SPECTRUM_RESOLUTION] = basicSpectrum[SPECTRUM_RESOLUTION - 1];
	}
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
	return statePallet[cast(state)];
}

Urho3D::Material* ColorPaletteRepo::getLineMaterial() const {
	return lineMaterial;
}

Urho3D::Material* ColorPaletteRepo::getInfluenceMaterial() const {
	return influenceMaterial;
}

std::tuple<bool, Urho3D::Color> ColorPaletteRepo::getInfoForGrid(CellState state) const {
	switch (state) {
	case CellState::NONE: return {false, Urho3D::Color(0, 0, 0, 0)};
	case CellState::ATTACK: return {true, Urho3D::Color::RED};
	case CellState::COLLECT: return {true, Urho3D::Color::YELLOW};
	case CellState::RESOURCE: return {true, Urho3D::Color::GREEN};
	case CellState::BUILDING: return {true, Urho3D::Color::BLUE};
	case CellState::DEPLOY: return {true, Urho3D::Color::CYAN};
	default: ;
	}
}

Urho3D::Color ColorPaletteRepo::getCircleColor(db_building* dbBuilding) const {
	if (dbBuilding->typeDefence) {
		return Urho3D::Color::BLACK;
	}
	if (dbBuilding->typeResourceWood) {
		return Urho3D::Color(0x00654321, Urho3D::Color::ARGB);
	}
	if (dbBuilding->typeResourceStone) {
		return Urho3D::Color::GRAY;
	}
	if (dbBuilding->typeResourceGold) {
		return Urho3D::Color::YELLOW;
	}
	return Urho3D::Color::CYAN;
}

Urho3D::Color ColorPaletteRepo::getColorForValidation(const db_building* building, Urho3D::Vector2& hitPos) const{
	const Environment* env = Game::getEnvironment();
	if (env->validateStatic(building->size, hitPos, true)) {
		if (env->isVisible(Game::getPlayersMan()->getActivePlayerID(), hitPos)) {
			if (Game::getPlayersMan()->getActivePlayer()->getResources().hasEnough(building->costs)) {
				return Urho3D::Color::GREEN;
			} 
			return Urho3D::Color(0.7, 0.4, 0.1);
		} 
		return Urho3D::Color(0.7, 0.2, 0.1);
	} 
	return Urho3D::Color::RED;
}
