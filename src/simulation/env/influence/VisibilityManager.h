#pragma once
#include <vector>

namespace Urho3D {
	class Vector2;
}

class ResourceEntity;
class Unit;
class Building;
struct GridCalculator;
class VisibilityMap;

class VisibilityManager {
public:
	VisibilityManager(char numberOfPlayers, float mapSize);
	~VisibilityManager();
	void updateVisibility(std::vector<Building*>* buildings, std::vector<Unit*>* units, std::vector<ResourceEntity*>* resources) const;
	void drawMaps(short currentDebugBatch, char index) const;
	bool isVisible(char player, const Urho3D::Vector2& pos) const;
	float getVisibilityScore(char player);
	void removeUnseen(char player, float* intersection);
private:
	std::vector<VisibilityMap*> visibilityPerPlayer;
	GridCalculator* calculator;
};
