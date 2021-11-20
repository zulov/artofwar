#include "VisibilityManager.h"

#include <iostream>
#include <Urho3D/Graphics/Material.h>
#include <Urho3D/Resource/ResourceCache.h>
#include <Urho3D/Graphics/Texture2D.h>
#include <Urho3D/Resource/Image.h>

#include "Game.h"
#include "MapsUtils.h"
#include "colors/ColorPaletteRepo.h"
#include "map/VisibilityMap.h"
#include "objects/building/Building.h"
#include "objects/unit/Unit.h"
#include "utils/DeleteUtils.h"
#include "map/VisibilityType.h"
#include "player/PlayersManager.h"
#include "simulation/SimGlobals.h"
#include "simulation/env/Environment.h"
#include "simulation/env/GridCalculatorProvider.h"


VisibilityManager::VisibilityManager(char numberOfPlayers, float mapSize, Urho3D::Terrain * terrain) {
	visibilityPerPlayer.reserve(numberOfPlayers);
	const unsigned short resolution = mapSize / VISIBILITY_GRID_FIELD_SIZE;

	for (int player = 0; player < numberOfPlayers; ++player) {
		visibilityPerPlayer.emplace_back(new VisibilityMap(resolution, mapSize, 3));
	}
	calculator = GridCalculatorProvider::get(resolution, mapSize);
	
	if (terrain && !SIM_GLOBALS.HEADLESS) {
		auto a = terrain->GetMaterial()->GetTexture(Urho3D::TextureUnit::TU_DIFFUSE);
		auto d = a->GetName();
		texture = Game::getCache()->GetResource<Urho3D::Texture2D>(d);
		image = texture->GetImage();
		dataCopy = new unsigned[texture->GetHeight() * texture->GetWidth()];
		auto* data = (unsigned*)image.Get()->GetData();
		std::copy_n(data, texture->GetHeight() * texture->GetWidth(), dataCopy);
	}
}

VisibilityManager::~VisibilityManager() {
	clear_vector(visibilityPerPlayer);
	delete[] dataCopy;
}

void VisibilityManager::setToImage(unsigned* data, std::initializer_list<int> indexes, unsigned color, bool operatorA) {
	for (const auto index : indexes) {
		const auto prev = *(data + index);

		if (operatorA) {
			*(data + index) = color & (*(dataCopy + index));
		} else {
			*(data + index) = color | (*(dataCopy + index));
		}

		if (*(data + index) != prev) {
			imageChanged = true;
		}
	}
}

void VisibilityManager::setToImage(unsigned* data, std::initializer_list<int> indexes, unsigned color) {
	for (const auto index : indexes) {
		if (*(data + index) != color) {
			imageChanged = true;
			*(data + index) = color;
		}
	}
}

void VisibilityManager::hideOrShow(VisibilityMap* current, Physical* physical) {
	auto pos = physical->getPosition();

	if (current->getValueAt(Urho3D::Vector2(pos.x_, pos.z_)) == static_cast<char>(VisibilityType::VISIBLE)) {
		physical->setVisibility(true);
	} else {
		physical->setVisibility(false);
	}
}

void VisibilityManager::updateVisibility(std::vector<Building*>* buildings, std::vector<Unit*>* units,
                                         std::vector<ResourceEntity*>* resources) {
	MapsUtils::resetMaps(visibilityPerPlayer);
	for (const auto unit : (*units)) {
		visibilityPerPlayer[unit->getPlayer()]->update(unit);
	}
	for (const auto building : (*buildings)) {
		visibilityPerPlayer[building->getPlayer()]->update(building);
	}
	const auto terrain = Game::getEnvironment()->getTerrain();

	if (terrain && !SIM_GLOBALS.HEADLESS) {
		imageChanged = false;
		auto current = visibilityPerPlayer[Game::getPlayersMan()->getActivePlayerID()];

		auto* data = (unsigned*)image.Get()->GetData();
		for (int i = 0; i < current->getResolution() * current->getResolution(); ++i) {

			auto cords = calculator->getIndexes(i);
			auto a = cords.x_ * 4 * calculator->getResolution() + cords.y_ * 2;
			auto b = a + 1;
			auto c = a + 2 * calculator->getResolution();
			auto d = c + 1;
			assert(d < texture->GetHeight()* texture->GetWidth());

			char currentValue = current->getValueAt(i);
			if (currentValue == static_cast<char>(VisibilityType::VISIBLE)) {
				setToImage(data, { a, b, c, d }, 0x00FFFFFF, true);
			}
			else if (currentValue == static_cast<char>(VisibilityType::SEEN)) {
				setToImage(data, { a, b, c, d }, 0xFF000000, false);
			}
			else {
				setToImage(data, { a, b, c, d }, 0xFF000000);
			}
		}
		if (imageChanged) {
			texture->SetData(image, true);
		}

		for (const auto resource : (*resources)) {
			hideOrShow(current, resource);
		}
		for (const auto unit : (*units)) {
			hideOrShow(current, unit);
		}
		for (const auto building : (*buildings)) {
			hideOrShow(current, building);
		}
	}
}

void VisibilityManager::drawMaps(short currentDebugBatch, char index) const {
	MapsUtils::drawMap(currentDebugBatch, index, visibilityPerPlayer);
}

bool VisibilityManager::isVisible(char player, const Urho3D::Vector2& pos) const {
	return visibilityPerPlayer[player]->getValueAt(pos) == static_cast<char>(VisibilityType::VISIBLE);
}

float VisibilityManager::getVisibilityScore(char player) {
	return visibilityPerPlayer[player]->getPercent();
}

void VisibilityManager::removeUnseen(char player, float* intersection) {
	visibilityPerPlayer[player]->removeUnseen(intersection);
}
