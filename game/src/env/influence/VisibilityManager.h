#pragma once
#include <cassert>
#include <span>
#include <vector>
#include <Urho3D/Container/Ptr.h>

#include "VisibilityMode.h"


class Physical;

namespace Urho3D {
	class Terrain;
	class Texture2D;
	class Image;
	class Vector2;
}

class ResourceEntity;
class Unit;
class Building;
class VisibilityMap;

namespace VisibilityTextureUtils {
	inline int getVisibilityIndex(int textureRow, int textureColumn, int textureHeight, int textureWidth,
	                              int visibilityResolution) {
		assert(textureRow >= 0 && textureRow < textureHeight);
		assert(textureColumn >= 0 && textureColumn < textureWidth);
		assert(textureHeight > 0 && textureWidth > 0 && visibilityResolution > 0);

		const int row = textureRow * visibilityResolution / textureHeight;
		const int column = textureColumn * visibilityResolution / textureWidth;
		return row * visibilityResolution + column;
	}
}

constexpr int CHANGED_INDEXES_MAX_SIZE = 100;

class VisibilityManager {
public:
	VisibilityManager(char numberOfPlayers, float mapSize, Urho3D::Terrain* terrain);
	~VisibilityManager();
	void setToImage(unsigned* data, int index, unsigned color);
	void setToImage(unsigned* data, int index, unsigned color, bool operatorA);
	void hideOrShow(VisibilityMap* current, Physical* physical);
	void updateVisibility(const std::vector<Building*>* buildings, const std::vector<Unit*>* units, const std::vector<ResourceEntity*>* resources);
	void drawMaps(short currentDebugBatch, char index) const;
	bool isVisible(char player, const Urho3D::Vector2& pos) const;
	float getVisibilityScore(char player) const;
	int removeUnseen(unsigned char player, std::span<float> intersection) const;
	void nextVisibilityType();
private:
	std::vector<VisibilityMap*> visibilityPerPlayer;
	Urho3D::SharedPtr<Urho3D::Image> image;
	unsigned* dataCopy{};
	Urho3D::Texture2D* texture{};
	VisibilityMode visibilityMode = VisibilityMode::PLAYER_ONLY;
	bool imageChanged;
};
