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
#include "player/PlayersManager.h"
#include "simulation/SimGlobals.h"
#include "env/Environment.h"
#include "utils/OtherUtils.h"
#include "objects/resource/ResourceEntity.h"
#include "utils/DeleteUtils.h"


VisibilityManager::VisibilityManager(char numberOfPlayers, float mapSize, Urho3D::Terrain* terrain) {
	visibilityPerPlayer.reserve(numberOfPlayers);
	const unsigned short resolution = mapSize / VISIBILITY_GRID_FIELD_SIZE;

	for (int player = 0; player < numberOfPlayers; ++player) {
		visibilityPerPlayer.emplace_back(new VisibilityMap(resolution, mapSize, 3));
	}

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

void VisibilityManager::setToImage(unsigned* data, int index, unsigned color, bool operatorA) {
	const auto previous = data[index];
	data[index] = operatorA ? color & dataCopy[index] : color | dataCopy[index];
	imageChanged |= data[index] != previous;
}

void VisibilityManager::setToImage(unsigned* data, int index, unsigned color) {
	imageChanged |= data[index] != color;
	data[index] = color;
}

void VisibilityManager::hideOrShow(VisibilityMap* current, Physical* physical) {
	VisibilityType type = VisibilityType::NONE;
	const auto pos = physical->getPosition();
	
	if (visibilityMode == VisibilityMode::ALL) {
		type = VisibilityType::VISIBLE;
	} else if (visibilityMode == VisibilityMode::ALL_PLAYERS) {
		char val = castC(type);
		for (auto vis : visibilityPerPlayer) {
			val |= castC(vis->getValueAt(pos.x_, pos.y_));
		}
		type = static_cast<VisibilityType>(val);
	} else {
		type = current->getValueAt(pos.x_,pos.y_);
	}
	physical->setVisibility(type);
}

void VisibilityManager::updateVisibility(const std::vector<Building*>* buildings, const std::vector<Unit*>* units,
                                         const std::vector<ResourceEntity*>* resources) {
	MapsUtils::resetMaps(visibilityPerPlayer);
	for (const auto unit : (*units)) {
		visibilityPerPlayer[unit->getPlayer()]->update(unit->getPosition(), unit->getSightRadius());
	}
	for (const auto building : (*buildings)) {
		visibilityPerPlayer[building->getPlayer()]->update(building->getPosition(), building->getSightRadius());
	}
	for (const auto perPlayer : visibilityPerPlayer) {
		perPlayer->finish();
	}
	const auto terrain = Game::getEnvironment()->getTerrain();

	if (terrain && !SIM_GLOBALS.HEADLESS) {
		imageChanged = false;
		auto current = visibilityPerPlayer[Game::getPlayersMan()->getActivePlayerID()];

		auto* data = (unsigned*)image.Get()->GetData();
		const int textureHeight = texture->GetHeight();
		const int textureWidth = texture->GetWidth();
		for (int row = 0; row < textureHeight; ++row) {
			for (int column = 0; column < textureWidth; ++column) {
				const int textureIndex = row * textureWidth + column;
				const int visibilityIndex = VisibilityTextureUtils::getVisibilityIndex(
					row, column, textureHeight, textureWidth, current->getResolution());
				if (visibilityMode == VisibilityMode::ALL) {
					setToImage(data, textureIndex, 0x00FFFFFF, true);
				} else {
					char combineTime = castC(VisibilityType::NONE);
					if (visibilityMode == VisibilityMode::ALL_PLAYERS) {
						for (auto vis : visibilityPerPlayer) {
							const char currentValue = vis->getValueAt(visibilityIndex);
							combineTime |= currentValue;
						}
					} else {
						combineTime = current->getValueAt(visibilityIndex);
					}
					VisibilityType currentValue = static_cast<VisibilityType>(combineTime);
					if (currentValue == VisibilityType::VISIBLE) {
						setToImage(data, textureIndex, 0x00FFFFFF, true);
					} else if (currentValue == VisibilityType::SEEN) {
						setToImage(data, textureIndex, 0xFF000000, false);
					} else {
						setToImage(data, textureIndex, 0xFF000000);
					}
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

float VisibilityManager::getVisibilityScore(char player) const {
	return visibilityPerPlayer[player]->getPercent();
}

int VisibilityManager::removeUnseen(unsigned char player, std::span<float> intersection) const {
	return visibilityPerPlayer[player]->removeUnseen(intersection);
}

void VisibilityManager::nextVisibilityType() {
	visibilityMode = NEXT_VISIBILITY_MODES[castC(visibilityMode)];
}
