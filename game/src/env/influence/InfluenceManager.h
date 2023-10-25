#pragma once

#include <span>
#include <array>
#include <optional>
#include <vector>
#include <Urho3D/Math/Vector3.h>

#include "player/ai/InfluenceDataType.h"

enum class ParentBuildingType : char;
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
	explicit InfluenceManager(char numberOfPlayers, float mapSize, Urho3D::Terrain* terrain);
	~InfluenceManager();
	void update(std::vector<Unit*>* units) const;
	void update(const std::vector<Building*>* buildings) const;
	void update(const std::vector<ResourceEntity*>* resources) const;

	void updateQuadUnits(const std::vector<Unit*>* units) const;
	void updateWithHistory() const;
	void updateNotInBonus(std::vector<Unit*>* units) const;
	void updateQuadOther() const;
	void updateVisibility(std::vector<Building*>* buildings, std::vector<Unit*>* units,
	                      std::vector<ResourceEntity*>* resources) const;

	void updateInfluenceHistoryReset() const;

	void draw(InfluenceDataType type, char index);
	void drawAll() const;

	content_info* getContentInfo(const Urho3D::Vector2& center, CellState state, int additionalInfos, bool* checks,
	                             int activePlayer);
	std::array<float, 5>& getInfluenceDataAt(char player, const Urho3D::Vector2& pos);
	std::vector<int> getIndexesIterative(const std::span<float> result, float tolerance, int min,
	                                     std::span<InfluenceMapFloat*> maps) const;

	std::vector<Urho3D::Vector2> getAreasIterative(const std::span<float> result, char player, float tolerance,
	                                               int min);

	float getFieldSize() const;
	std::vector<int>* getAreas(const std::span<float> result, ParentBuildingType type, char player);
	std::vector<int>* getAreasResBonus(char id, char player);

	void addCollect(Unit* unit, short resId, float value);
	void addAttack(char player, const Urho3D::Vector3& position, float value);
	std::optional<Urho3D::Vector2> getCenterOf(CenterType id, char player);
	bool isVisible(char player, const Urho3D::Vector2& pos) const;
	Urho3D::Vector2 getCenter(int index) const;
	float getVisibilityScore(char player) const;
	int getIndex(const Urho3D::Vector3& position) const;
	void nextVisibilityType() const;

private:
	std::vector<int>* getAreas(std::span<InfluenceMapFloat*> maps, const std::span<float> result, char player) const;

	std::vector<int>* bestIndexes(float* values, const std::vector<unsigned>& indexes, float minVal) const;
	std::vector<Urho3D::Vector2> centersFromIndexes(const std::vector<int>& intersection) const;

	//TODO imprve nie vectory tylko obiekt z mapami per player

	std::vector<std::array<InfluenceMapFloat*, 3>> mapsForAiArmyPerPlayer;
	std::vector<std::array<InfluenceMapFloat*, 8>> mapsForAiPerPlayer;
	std::vector<std::array<InfluenceMapFloat*, 4>> mapsGatherSpeedPerPlayer;

	std::vector<std::array<InfluenceMapInt*, 4>> mapsResNotInBonusPerPlayer;

	std::vector<std::array<InfluenceMapQuad*, 3>> mapsForCentersPerPlayer;

	std::vector<InfluenceMapInt*> unitsNumberPerPlayer;

	std::vector<InfluenceMapFloat*> buildingsInfluencePerPlayer;
	std::vector<InfluenceMapFloat*> unitsInfluencePerPlayer;
	InfluenceMapFloat* resourceInfluence;

	std::vector<InfluenceMapHistory*> gatherSpeed[4];
	std::vector<InfluenceMapInt*> resNotInBonus[4];

	std::vector<InfluenceMapHistory*> attackSpeed;

	std::vector<InfluenceMapQuad*> econQuad;
	std::vector<InfluenceMapQuad*> buildingsQuad;
	std::vector<InfluenceMapQuad*> armyQuad;

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
