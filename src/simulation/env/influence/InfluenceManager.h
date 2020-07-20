#pragma once

#include <span>
#include <vector>

#include "map/InfluenceMapCombine.h"
#include "map/InfluenceMapFloat.h"
#include "map/InfluenceMapInt.h"
#include "objects/Physical.h"
#include "player/ai/ActionMaker.h"
#include "player/ai/InfluenceType.h"

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
	void drawMap(char index, const std::vector<InfluenceMapCombine*>& vector, ValueType type) const;
	
	void draw(InfluenceType type, char index);
	void switchDebug();

	content_info* getContentInfo(const Urho3D::Vector2& center, CellState state, int additionalInfos, bool* checks,
	                             int activePlayer);
	std::vector<float>& getInfluenceDataAt(char player, const Urho3D::Vector2& pos);
	std::vector<int> getIndexesIterative(const std::span<float> result, float tolerance, int min,
	                                     std::vector<InfluenceMapFloat*>& maps) const;

	std::vector<Urho3D::Vector2> getAreasIterative(const std::span<float> result, char player, float tolerance,
	                                               int min);

	float getFieldSize();
	std::vector<Urho3D::Vector2> getAreas(const std::span<float> result, char player);

private:
	std::vector<Urho3D::Vector2> centersFromIndexes(InfluenceMapFloat* map, float* values,
	                                                const std::vector<unsigned>& indexes, float minVal) const;
	std::vector<Urho3D::Vector2> centersFromIndexes(InfluenceMapFloat* map, const std::vector<int>& intersection);

	void resetMaps(const std::vector<InfluenceMapFloat*>& maps) const;
	void resetMaps(const std::vector<InfluenceMapInt*>& maps) const;
	void resetMaps(const std::vector<InfluenceMapCombine*>& maps) const;
	void calcStats(const std::vector<InfluenceMapFloat*>& maps) const;
	void calcStats(const std::vector<InfluenceMapInt*>& maps) const;
	void calcStats(const std::vector<InfluenceMapCombine*>& maps) const;

	void basicValuesFunc(float *weights, Physical* thing) const;

	std::vector<std::vector<InfluenceMapFloat*>> mapsForAiPerPlayer;

	std::vector<InfluenceMapInt*> unitsNumberPerPlayer;
	std::vector<InfluenceMapFloat*> buildingsInfluencePerPlayer;
	std::vector<InfluenceMapFloat*> unitsInfluencePerPlayer;
	std::vector<InfluenceMapFloat*> resourceInfluence;

	std::vector<InfluenceMapCombine*> basicValues;
	InfluenceMapCombine* main;

	InfluenceType debugType = InfluenceType::UNITS_INFLUENCE_PER_PLAYER;
	content_info* ci;
	std::vector<float> dataFromPos;
	short currentDebugBatch = 0;
};

