#pragma once
#include <vector>

#include "objects/building/Building.h"
#include "objects/unit/Unit.h"

class VisibilityMap;

class VisibilityManager {
public:
	VisibilityManager(char numberOfPlayers, float mapSize);
	~VisibilityManager();
	void updateVisibility(std::vector<Building*>* buildings, std::vector<Unit*>* units) const;
	void drawMaps(short currentDebugBatch, char index) const;
	bool isVisible(char player, const Urho3D::Vector2& pos) const;
	float getVisibilityScore(char player);
	void removeUnseen(char player, float* intersection);
private:
	std::vector<VisibilityMap*> visibilityPerPlayer;
	
};
