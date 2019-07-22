#pragma once

#include <vector>
#include "map/InfluenceMapInt.h"
#define DEFAULT_INF_GRID_SIZE 256

class Unit;
class content_info;

class InfluenceManager {
public:
	InfluenceManager(char numberOfPlayers);
	~InfluenceManager();
	void update(std::vector<Unit*>* units) const;
	content_info* getContentInfo(const Urho3D::Vector2& from, const Urho3D::Vector2& to, bool* checks,
	                             int activePlayer);
private:
	std::vector<InfluenceMapInt*> unitsNumberPerPlayer;
	content_info* ci;
};
