#pragma once

#include <vector>
#include "map/InfluenceMapInt.h"
#define DEFAULT_INF_GRID_SIZE 256

class Unit;

class InfluenceManager {
public:
	InfluenceManager(char numberOfPlayers);
	~InfluenceManager();
	void update(std::vector<Unit*>* units);
private:
	std::vector<InfluenceMapInt*> unitsNumberPerPlayer;
};
