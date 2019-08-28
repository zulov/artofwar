#pragma once

#include "map/InfluenceMapInt.h"
#include "map/InfluenceMapFloat.h"
#include "player/ai/InfluanceType.h"
#include <vector>

#define DEFAULT_INF_GRID_SIZE 128
#define DEFAULT_INF_FLOAT_GRID_SIZE 128
#define MAX_DEBUG_PARTS_INFLUENCE 32

enum class CellState : char;
class Unit;
class Building;
class ResourceEntity;
class content_info;

class InfluenceManager {
public:
	InfluenceManager(char numberOfPlayers);
	~InfluenceManager();
	void update(std::vector<Unit*>* units) const;
	void update(std::vector<Building*>* buildings) const;
	void update(std::vector<ResourceEntity*>* resources) const;
	void draw(InfluanceType type, char index);
	void switchDebug();

	content_info* getContentInfo(const Urho3D::Vector2& center, CellState state, int additionalInfos, bool* checks,
	                             int activePlayer);
	Urho3D::Vector2 getNewBuildingPos(const char player, const short id);
private:
	std::vector<InfluenceMapInt*> unitsNumberPerPlayer;
	std::vector<InfluenceMapFloat*> buildingsInfluencePerPlayer;
	std::vector<InfluenceMapFloat*> unitsInfluencePerPlayer;
	InfluanceType debugType = InfluanceType::UNITS_INFLUENCE_PER_PLAYER;
	char debugIndex = 0;
	content_info* ci;
	short currentDebugBatch = 0;
};
