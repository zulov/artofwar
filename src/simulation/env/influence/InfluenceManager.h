#pragma once

#include <span>
#include <array>
#include <optional>
#include <vector>
#include "player/ai/InfluenceDataType.h"

class VisibilityMap;
enum class CenterType:char;
struct GridCalculator;
class InfluenceMapInt;
class Building;
class ResourceEntity;
class InfluenceMapFloat;
class InfluenceMapCombine;
class InfluenceMapQuad;
class InfluenceMapHistory;

namespace Urho3D {
	class String;
	class Vector2;
}

enum class CellState : char;
class Unit;
struct content_info;

class InfluenceManager {
public:

	explicit InfluenceManager(char numberOfPlayers, float mapSize);
	~InfluenceManager();
	void update(std::vector<Unit*>* units) const;
	void update(std::vector<Building*>* buildings) const;
	void update(std::vector<ResourceEntity*>* resources) const;

	void updateQuadUnits(std::vector<Unit*>* units) const;
	void updateQuadBuildings(std::vector<Building*>* buildings) const;
	void updateWithHistory() const;
	void updateQuadOther() const;
	void updateVisibility(std::vector<Building*>* buildings, std::vector<Unit*>* units) const;

	void draw(InfluenceDataType type, char index);
	void drawAll() const;
	void switchDebug();

	content_info* getContentInfo(const Urho3D::Vector2& center, CellState state, int additionalInfos, bool* checks,
	                             int activePlayer);
	std::array<float, 5>& getInfluenceDataAt(char player, const Urho3D::Vector2& pos);
	std::vector<int> getIndexesIterative(const std::span<float> result, float tolerance, int min,
	                                     std::array<InfluenceMapFloat*, 5>& maps) const;

	std::vector<Urho3D::Vector2> getAreasIterative(const std::span<float> result, char player, float tolerance,
	                                               int min);

	float getFieldSize() const;
	std::vector<int> getAreas(const std::span<float> result, char player);
	void addCollect(Unit* unit, float value);
	void addAttack(Unit* unit, float value);
	std::optional<Urho3D::Vector2> getCenterOf(CenterType id, char player);
	bool isVisible(char player, const Urho3D::Vector2& pos);

private:
	std::vector<int> centersFromIndexes(float* values, const std::vector<unsigned>& indexes, float minVal) const;
	std::vector<Urho3D::Vector2> centersFromIndexes(const std::vector<int>& intersection) const;

	template <typename T>
	void resetMaps(const std::vector<T*>& maps) const;
	template <typename T>
	void finalize(const std::vector<T*>& maps) const;
	template <typename T>
	void drawMap(char index, const std::vector<T*>& maps) const;
	template <typename T>
	void drawAll(const std::vector<T*>& maps, Urho3D::String name) const;
	
	std::vector<std::array<InfluenceMapFloat*, 5>> mapsForAiPerPlayer;
	std::vector<std::array<InfluenceMapQuad*, 3>> mapsForCentersPerPlayer;

	std::vector<InfluenceMapInt*> unitsNumberPerPlayer;

	std::vector<InfluenceMapFloat*> buildingsInfluencePerPlayer;
	std::vector<InfluenceMapFloat*> unitsInfluencePerPlayer;
	InfluenceMapFloat* resourceInfluence;

	std::vector<InfluenceMapHistory*> gatherSpeed;
	std::vector<InfluenceMapHistory*> attackSpeed;

	std::vector<InfluenceMapQuad*> econQuad;
	std::vector<InfluenceMapQuad*> buildingsQuad;
	std::vector<InfluenceMapQuad*> unitsQuad;
	
	std::vector<VisibilityMap*> visibilityPerPlayer;

	content_info* ci;
	GridCalculator* calculator;
	std::array<float, 5> dataFromPos;
	short currentDebugBatch = 0;
	InfluenceDataType debugType = InfluenceDataType::UNITS_INFLUENCE_PER_PLAYER;
	unsigned int arraySize;
	float* intersection;// [arraySize] ;
};
