#include "VisibilityManager.h"

#include <Urho3D/Graphics/Material.h>
#include <Urho3D/Resource/ResourceCache.h>
#include <Urho3D/Graphics/Texture2D.h>
#include <Urho3D/Resource/Image.h>

#include "Game.h"
#include "MapsUtils.h"
#include "colors/ColorPaletteRepo.h"
#include "map/VisibilityMap.h"
#include "objects/unit/Unit.h"
#include "math/MathUtils.h"
#include "player/PlayersManager.h"
#include "simulation/SimGlobals.h"
#include "env/Environment.h"
#include "env/GridCalculatorProvider.h"
#include "utils/OtherUtils.h"
#include "objects/resource/ResourceEntity.h"


VisibilityManager::VisibilityManager(char numberOfPlayers, float mapSize, Urho3D::Terrain* terrain) {
	visibilityPerPlayer.reserve(numberOfPlayers);
	const unsigned short resolution = mapSize / VISIBILITY_GRID_FIELD_SIZE;

	for (int player = 0; player < numberOfPlayers; ++player) {
		visibilityPerPlayer.emplace_back(new VisibilityMap(resolution, mapSize, 3));
	}
	calculator = GridCalculatorProvider::get(resolution, mapSize);

	if (terrain && !SIM_GLOBALS.HEADLESS) {
		const auto name = terrain->GetMaterial()->GetTexture(Urho3D::TextureUnit::TU_DIFFUSE)->GetName();
		texture = Game::getCache()->GetResource<Urho3D::Texture2D>(name);
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

void VisibilityManager::setToImage(unsigned* data, std::array<int, 4>& indexes, unsigned color, bool operatorA) {
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

void VisibilityManager::setToImage(unsigned* data, std::array<int, 4>& indexes, unsigned color) {
	for (const auto index : indexes) {
		if (*(data + index) != color) {
			imageChanged = true;
			*(data + index) = color;
		}
	}
}

void VisibilityManager::hideOrShow(VisibilityMap* current, Physical* physical) {
	VisibilityType type = VisibilityType::NONE;
	const auto pos = physical->getPosition();
	
	if (visibilityMode == VisibilityMode::ALL) {
		type = VisibilityType::VISIBLE;
	} else if (visibilityMode == VisibilityMode::ALL_PLAYERS) {
		char val = cast(type);
		for (auto vis : visibilityPerPlayer) {
			val |= cast(vis->getValueAt(pos.x_, pos.z_));
		}
		type = static_cast<VisibilityType>(val);
	} else {
		type = current->getValueAt(pos.x_,pos.z_);
	}
	physical->setVisibility(type);
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
			auto parentIndexes = getCordsInHigher(calculator->getResolution(), i);
			assert(parentIndexes[3] < texture->GetHeight()* texture->GetWidth());
			if (visibilityMode == VisibilityMode::ALL) {
				setToImage(data, parentIndexes, 0x00FFFFFF, true);
			} else {
				char combineTime = cast(VisibilityType::NONE);
				if (visibilityMode == VisibilityMode::ALL_PLAYERS) {
					for (auto vis : visibilityPerPlayer) {
						const char currentValue = vis->getValueAt(i);
						combineTime |= currentValue;
					}
				} else {
					combineTime = current->getValueAt(i);
				}
				VisibilityType currentValue = static_cast<VisibilityType>(combineTime);
				if (currentValue == VisibilityType::VISIBLE) {
					setToImage(data, parentIndexes, 0x00FFFFFF, true);
				} else if (currentValue == VisibilityType::SEEN) {
					setToImage(data, parentIndexes, 0xFF000000, false);
				} else {
					setToImage(data, parentIndexes, 0xFF000000);
				}
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

void VisibilityManager::nextVisibilityType() {
	visibilityMode = NEXT_VISIBILITY_MODES[cast(visibilityMode)];
}
