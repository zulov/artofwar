#include "ColorPaletteRepo.h"

#include <Urho3D/Graphics/Material.h>
#include <Urho3D/Resource/ResourceCache.h>
#include "Game.h"
#include "database/db_strcut.h"
#include "math/MathUtils.h"
#include "env/bucket/CellEnums.h"
#include "player/Player.h"
#include "player/PlayersManager.h"
#include "simulation/SimGlobals.h"
#include "env/Environment.h"
#include "player/Resources.h"

ColorPaletteRepo::ColorPaletteRepo() {
	if (!SIM_GLOBALS.HEADLESS) {
		lineMaterial = Game::getCache()->GetResource<Urho3D::Material>("Materials/line.xml");
		influenceMaterial = Game::getCache()->GetResource<Urho3D::Material>("Materials/influence.xml");
		for (int i = 0; i < SPECTRUM_RESOLUTION; ++i) {
			basicSpectrum[i] = Urho3D::Color(0.1, i * (1.0f / SPECTRUM_RESOLUTION), 0.1, 0.5);
			basicSpectrumSolid[i] = Urho3D::Color(0.1, i * (1.0f / SPECTRUM_RESOLUTION), 0.1, 1);
		}
		basicSpectrum[SPECTRUM_RESOLUTION] = basicSpectrum[SPECTRUM_RESOLUTION - 1];
		basicSpectrumSolid[SPECTRUM_RESOLUTION] = basicSpectrumSolid[SPECTRUM_RESOLUTION - 1];
	}
}

Urho3D::Color& ColorPaletteRepo::getColor(float value, float maxValue) {
	value = fixValue(value, maxValue);
	const int index = value / maxValue * SPECTRUM_RESOLUTION;
	return basicSpectrum[index];
}
Urho3D::Color& ColorPaletteRepo::getSolidColor(float value, float maxValue) {
	value = fixValue(value, maxValue);
	const int index = value / maxValue * SPECTRUM_RESOLUTION;
	return basicSpectrumSolid[index];
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
			if (Game::getPlayersMan()->getActivePlayer()->getResources()->hasEnough(building->costs)) {
				return Urho3D::Color::GREEN;
			} 
			return Urho3D::Color(0.7, 0.4, 0.1);
		} 
		return Urho3D::Color(0.7, 0.2, 0.1);
	} 
	return Urho3D::Color::RED;
}
