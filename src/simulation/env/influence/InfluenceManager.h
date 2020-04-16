#pragma once

#include "map/InfluenceMapInt.h"
#include "map/InfluenceMapFloat.h"
#include "player/ai/InfluenceType.h"
#include <vector>

#define DEFAULT_INF_GRID_SIZE 128
#define DEFAULT_INF_FLOAT_GRID_SIZE 128
#define MAX_DEBUG_PARTS_INFLUENCE 32

enum class CellState : char;
class Unit;
class Building;
class ResourceEntity;
struct content_info;

class InfluenceManager {
public:
	explicit InfluenceManager(char numberOfPlayers);
	~InfluenceManager();
	void update(std::vector<Unit*>* units) const;
	void update(std::vector<Building*>* buildings) const;

	void update(std::vector<ResourceEntity*>* resources) const;

	void update(std::vector<Unit*>* units, std::vector<Building*>* buildings) const;
	void drawMap(char index, const std::vector<InfluenceMapFloat*>& vector) const;

	void draw(InfluenceType type, char index);
	void switchDebug();

	content_info* getContentInfo(const Urho3D::Vector2& center, CellState state, int additionalInfos, bool* checks,
	                             int activePlayer);
	void writeInInfluenceDataAt(float* data, char player, const Urho3D::Vector2& pos);
	std::vector<Urho3D::Vector2> getAreas(float* result, char player, float tolerance);

private:
	void resetMapsF(const std::vector<InfluenceMapFloat*>& maps) const;
	void resetMapsI(const std::vector<InfluenceMapInt*>& maps) const;
	void calcStats(const std::vector<InfluenceMapFloat*>& maps) const;
	void calcStats(const std::vector<InfluenceMapInt*>& maps) const;

	std::vector<InfluenceMapInt*> unitsNumberPerPlayer;
	std::vector<InfluenceMapFloat*> buildingsInfluencePerPlayer;
	std::vector<InfluenceMapFloat*> unitsInfluencePerPlayer;
	std::vector<InfluenceMapFloat*> resourceInfluence;

	std::vector<InfluenceMapFloat*> econLevelPerPlayer;
	std::vector<InfluenceMapFloat*> attackLevelPerPlayer;
	std::vector<InfluenceMapFloat*> defenceLevelPerPlayer;

	InfluenceType debugType = InfluenceType::UNITS_INFLUENCE_PER_PLAYER;
	content_info* ci;
	short currentDebugBatch = 0;
};
