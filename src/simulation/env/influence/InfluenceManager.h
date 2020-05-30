#pragma once

#include "map/InfluenceMapInt.h"
#include "map/InfluenceMapFloat.h"
#include "player/ai/InfluenceType.h"
#include <vector>

#define DEFAULT_INF_GRID_SIZE 128
#define DEFAULT_INF_FLOAT_GRID_SIZE 128
#define MAX_DEBUG_PARTS_INFLUENCE 32

enum class AiInfluenceType : char;
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
	std::vector<float>& getInfluenceDataAt(char player, const Urho3D::Vector2& pos);
	std::vector<int> getIndexesIterative(const std::vector<float>& result, float tolerance, int min,
	                                     std::vector<InfluenceMapFloat*>& maps) const;

	std::vector<Urho3D::Vector2> getAreasIterative(const std::vector<float>& result, char player, float tolerance,
	                                               int min);

	float getFieldSize();
	std::vector<Urho3D::Vector2> getAreas(const std::vector<float>& result, char player,
	                                       std::vector<float> tolerances);

private:

	std::vector<Urho3D::Vector2> centersFromIndexes(InfluenceMapFloat* map, unsigned char* values,
	                                                const std::vector<unsigned>& indexes, unsigned char minVal) const;
	std::vector<Urho3D::Vector2> centersFromIndexes(InfluenceMapFloat* map, const std::vector<int>& intersection);

	void resetMapsF(const std::vector<InfluenceMapFloat*>& maps) const;
	void resetMapsI(const std::vector<InfluenceMapInt*>& maps) const;
	void calcStats(const std::vector<InfluenceMapFloat*>& maps) const;
	void calcStats(const std::vector<InfluenceMapInt*>& maps) const;

	std::vector<std::vector<InfluenceMapFloat*>> mapsForAiPerPlayer;

	std::vector<InfluenceMapInt*> unitsNumberPerPlayer;
	std::vector<InfluenceMapFloat*> buildingsInfluencePerPlayer;
	std::vector<InfluenceMapFloat*> unitsInfluencePerPlayer;
	std::vector<InfluenceMapFloat*> resourceInfluence;

	std::vector<InfluenceMapFloat*> econLevelPerPlayer;
	std::vector<InfluenceMapFloat*> attackLevelPerPlayer;
	std::vector<InfluenceMapFloat*> defenceLevelPerPlayer;

	InfluenceType debugType = InfluenceType::UNITS_INFLUENCE_PER_PLAYER;
	content_info* ci;
	std::vector<float> dataFromPos;
	short currentDebugBatch = 0;
};
