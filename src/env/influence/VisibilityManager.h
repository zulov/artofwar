#pragma once
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
struct GridCalculator;
class VisibilityMap;

class VisibilityManager {
public:
	VisibilityManager(char numberOfPlayers, float mapSize, Urho3D::Terrain* terrain);
	~VisibilityManager();
	void setToImage(unsigned* data, std::array<int, 4>& indexes, unsigned color);
	void setToImage(unsigned* data, std::array<int, 4>& indexes, unsigned color, bool operatorA);
	void hideOrShow(VisibilityMap* current, Physical* physical);
	void updateVisibility(std::vector<Building*>* buildings, std::vector<Unit*>* units, std::vector<ResourceEntity*>* resources);
	void drawMaps(short currentDebugBatch, char index) const;
	bool isVisible(char player, const Urho3D::Vector2& pos) const;
	float getVisibilityScore(char player);
	void removeUnseen(char player, float* intersection);
	void nextVisibilityType();
private:
	std::vector<VisibilityMap*> visibilityPerPlayer;
	GridCalculator* calculator;
	Urho3D::SharedPtr<Urho3D::Image> image;
	unsigned* dataCopy{};
	Urho3D::Texture2D* texture{};
	VisibilityMode visibilityMode = VisibilityMode::PLAYER_ONLY;
	bool imageChanged;
};
