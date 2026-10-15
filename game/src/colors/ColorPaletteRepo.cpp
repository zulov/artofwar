#include "ColorPaletteRepo.h"

#include <Urho3D/Graphics/Material.h>
#include <Urho3D/Resource/ResourceCache.h>
#include "Game.h"
#include "database/db_struct.h"
#include "math/MathUtils.h"
#include "env/bucket/CellEnums.h"
#include "player/Player.h"
#include "player/PlayersManager.h"
#include "simulation/SimGlobals.h"
#include "env/Environment.h"
#include "player/Resources.h"

const Urho3D::Color ColorPaletteRepo::FPS_NEUTRAL(0.85f, 0.85f, 0.85f);
const Urho3D::Color ColorPaletteRepo::FPS_LOW_RANK_COLORS[3] = {
	Urho3D::Color(1.0f, 0.42f, 0.42f), // Coral red
	Urho3D::Color(1.0f, 0.62f, 0.26f), // Orange
	Urho3D::Color(1.0f, 0.82f, 0.40f)  // Amber
};
const Urho3D::Color ColorPaletteRepo::FPS_HIGH_RANK_COLORS[3] = {
	Urho3D::Color(0.72f, 0.89f, 0.66f), // Light green
	Urho3D::Color(0.51f, 0.83f, 0.60f), // Medium green
	Urho3D::Color(0.30f, 0.80f, 0.50f)  // Emerald
};

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

const Urho3D::Color& ColorPaletteRepo::getFpsRankColor(unsigned rank) {
	return FPS_LOW_RANK_COLORS[rank];
}

const Urho3D::Color& ColorPaletteRepo::getFpsBestRankColor(unsigned rank) {
	return FPS_HIGH_RANK_COLORS[rank];
}

const Urho3D::Color& ColorPaletteRepo::getFpsNeutralColor() {
	return FPS_NEUTRAL;
}

const Urho3D::Color& ColorPaletteRepo::getColor(float value, float maxValue) const {
	value = fixValue(value, maxValue);
	const int index = value / maxValue * SPECTRUM_RESOLUTION;
	return basicSpectrum[index];
}
const Urho3D::Color& ColorPaletteRepo::getSolidColor(float value, float maxValue) const {
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

Urho3D::Color ColorPaletteRepo::getInfoForGrid(CellState state) const {
	switch (state) {
	case CellState::NONE: return Urho3D::Color(0, 0, 0, 0);
	case CellState::ATTACK: return Urho3D::Color::RED;
	case CellState::COLLECT: return Urho3D::Color::YELLOW;
	case CellState::RESOURCE: return Urho3D::Color::GREEN;
	case CellState::BUILDING: return Urho3D::Color::BLUE;
	case CellState::DEPLOY: return Urho3D::Color::CYAN;
	default: ;
	}
	return Urho3D::Color(0, 0, 0, 0);
}

Urho3D::Color ColorPaletteRepo::getCircleColor(const db_building* dbBuilding) const {
	if (dbBuilding->typeDefence) {
		return Urho3D::Color::BLACK;
	}
	if (dbBuilding->hasResourceType(ResourceType::WOOD)) {
		return Urho3D::Color(0x00654321, Urho3D::Color::ARGB);
	}
	if (dbBuilding->hasResourceType(ResourceType::STONE)) {
		return Urho3D::Color::GRAY;
	}
	if (dbBuilding->hasResourceType(ResourceType::GOLD)) {
		return Urho3D::Color::YELLOW;
	}
	return Urho3D::Color::CYAN;
}

Urho3D::Color ColorPaletteRepo::getColorForValidation(const db_building* building, const Urho3D::Vector2& hitPos) const {
	const Environment* env = Game::getEnvironment();
	if (env->validateStatic(building->size, hitPos, true)) {
		if (env->isVisible(Game::getPlayersMan()->getActivePlayerID(), hitPos)) {
			if (Game::getPlayersMan()->getActivePlayer()->getResources()->hasEnough(building)) {
				return Urho3D::Color::GREEN;
			} 
			return Urho3D::Color(0.7, 0.4, 0.1);
		} 
		return Urho3D::Color(0.7, 0.2, 0.1);
	} 
	return Urho3D::Color::RED;
}
