#pragma once

#include <span>
#include <array>
#include <optional>
#include <vector>
#include <Urho3D/Math/Vector3.h>
#include <Urho3D/Math/Vector3.h>

#include "player/ai/InfluenceDataType.h"

class VisibilityManager;
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
	class Terrain;
	class String;
	class Vector2;
}

enum class CellState : char;
class Unit;
struct content_info;

class InfluenceManager {
public:
	explicit InfluenceManager(char numberOfPlayers, float mapSize, Urho3D::Terrain * terrain);
	~InfluenceManager();
	void update(std::vector<Unit*>* units) const;
	void update(std::vector<Building*>* buildings) const;
	void update(std::vector<ResourceEntity*>* resources) const;

	void updateQuadUnits(std::vector<Unit*>* units) const;
	void updateWithHistory() const;
	void updateQuadOther() const;
	void updateVisibility(std::vector<Building*>* buildings, std::vector<Unit*>* units,
	                      std::vector<ResourceEntity*>* resources) const;

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
	std::vector<int>* getAreas(const std::span<float> result, char player);
	void addCollect(Unit* unit, float value);
	void addAttack(char player, const Urho3D::Vector3& position, float value);
	std::optional<Urho3D::Vector2> getCenterOf(CenterType id, char player);
	bool isVisible(char player, const Urho3D::Vector2& pos) const;
	Urho3D::Vector2 getCenter(int index) const;
	float getVisibilityScore(char player) const;
	int getIndex(const Urho3D::Vector3& position) const;

private:
	std::vector<int>* centersFromIndexes(float* values, const std::vector<unsigned>& indexes, float minVal) const;
	std::vector<Urho3D::Vector2> centersFromIndexes(const std::vector<int>& intersection) const;

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

	VisibilityManager* visibilityManager;

	content_info* ci;
	GridCalculator* calculator;
	std::array<float, 5> dataFromPos;
	short currentDebugBatch = 0;
	InfluenceDataType debugType = InfluenceDataType::UNITS_INFLUENCE_PER_PLAYER;
	unsigned int arraySize;
	float* intersection; // [arraySize] ;
	std::vector<int>* tempIndexes;
};
