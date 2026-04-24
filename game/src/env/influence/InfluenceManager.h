#pragma once

#include <span>
#include <array>
#include <optional>
#include <vector>
#include <Urho3D/Math/Vector3.h>

#include "objects/resource/ResourceType.h"
#include "player/ai/InfluenceDataType.h"

enum class ParentBuildingType : char;
enum class CenterType : char;
enum class CellState : char;
struct GridCalculator;
struct content_info;
class InfluenceMapInt;
class Building;
class ResourceEntity;
class InfluenceMapFloat;
class InfluenceMapQuad;
class InfluenceMapHistory;
class VisibilityManager;
class Unit;

namespace Urho3D {
	class Terrain;
}

inline constexpr int AI_MAP_COUNT = 8;
inline constexpr int AI_ARMY_MAP_COUNT = 3;
inline constexpr int CENTER_TYPE_COUNT = 3;

class InfluenceManager {
public:
	explicit InfluenceManager(unsigned char numberOfPlayers, float mapSize, Urho3D::Terrain* terrain);
	~InfluenceManager();

	// --- Update (called from game loop) ---
	void updateUnits(std::vector<Unit*>* units) const;
	void updateBuildings(const std::vector<Building*>* buildings) const;
	void updateResources(const std::vector<ResourceEntity*>* resources) const;
	void updateQuadUnits(const std::vector<Unit*>* units) const;
	void updateQuadOther() const;
	void updateWithHistory() const;
	void updateNotInBonus(std::vector<Unit*>* units) const;
	void updateVisibility(std::vector<Building*>* buildings, std::vector<Unit*>* units,
	                      std::vector<ResourceEntity*>* resources) const;
	void resetHistoryThresholds() const;

	// --- Events (called during simulation) ---
	void addCollect(Unit* unit, short resId, float value) const;
	void addAttack(unsigned char player, const Urho3D::Vector3& position, float value) const;

	// --- AI queries ---
	std::optional<Urho3D::Vector2> getCenterOf(CenterType id, unsigned char player) const;
	std::vector<unsigned>* getAreas(std::span<const float> result, ParentBuildingType type, unsigned char player) const;
	std::vector<unsigned> getAreasResBonus(unsigned char resId, unsigned char player) const;
	std::vector<Urho3D::Vector2> getAreasIterative(std::span<const float> result, unsigned char player,
	                                               float tolerance, int min) const;

	// --- Utility ---
	bool isVisible(unsigned char player, const Urho3D::Vector2& pos) const;
	float getVisibilityScore(unsigned char player) const;
	Urho3D::Vector2 getCenter(int index) const;
	int getIndex(const Urho3D::Vector3& position) const;
	float getFieldSize() const;

	// --- Debug / UI ---
	void draw(InfluenceDataType type, unsigned char index);
	void drawAll() const;
	void nextVisibilityType() const;
	content_info* getContentInfo(const Urho3D::Vector2& center, CellState state, int additionalInfos, bool* checks,
	                             int activePlayer) const;

private:
	// --- Private helpers ---
	std::vector<unsigned>* getAreas(std::span<InfluenceMapFloat* const> maps, std::span<const float> result,
	                                unsigned char player) const;
	std::vector<int> getIndexesIterative(std::span<const float> result, float tolerance, int min,
	                                     std::span<InfluenceMapFloat* const> maps) const;
	std::vector<unsigned>* bestIndexes(float* values, const std::vector<unsigned>& indexes, float minVal) const;
	std::vector<Urho3D::Vector2> centersFromIndexes(const std::vector<int>& intersection) const;
	int getIndexInInfluence(Unit* unit) const;

	// --- Per-player influence maps ---
	std::vector<InfluenceMapFloat*> buildingsInfluencePerPlayer;
	std::vector<InfluenceMapFloat*> unitsInfluencePerPlayer;
	std::vector<InfluenceMapInt*> unitsNumberPerPlayer;
	std::vector<InfluenceMapHistory*> attackSpeed;
	std::vector<InfluenceMapHistory*> gatherSpeed[RESOURCES_SIZE];
	std::vector<InfluenceMapInt*> resNotInBonus[RESOURCES_SIZE];

	// --- Shared maps ---
	InfluenceMapFloat* resourceInfluence;

	// --- Quadtree center maps (per player) ---
	std::vector<InfluenceMapQuad*> econQuad;
	std::vector<InfluenceMapQuad*> buildingsQuad;
	std::vector<InfluenceMapQuad*> armyQuad;

	// --- AI map views (non-owning, per player) ---
	std::vector<std::array<InfluenceMapFloat*, AI_MAP_COUNT>> mapsForAiPerPlayer;
	std::vector<std::array<InfluenceMapFloat*, AI_ARMY_MAP_COUNT>> mapsForAiArmyPerPlayer;
	std::vector<std::array<InfluenceMapFloat*, RESOURCES_SIZE>> mapsGatherSpeedPerPlayer;
	std::vector<std::array<InfluenceMapInt*, RESOURCES_SIZE>> mapsResNotInBonusPerPlayer;
	std::vector<std::array<InfluenceMapQuad*, CENTER_TYPE_COUNT>> mapsForCentersPerPlayer;

	// --- Infrastructure ---
	GridCalculator* calculator;
	VisibilityManager* visibilityManager;
	float* sharedTemplateV;

	// --- Scratch buffers ---
	mutable content_info* ci;
	mutable unsigned int arraySize;
	mutable float* intersection; // [arraySize]
	mutable std::vector<unsigned> tempIndexes;

	// --- Debug state ---
	short currentDebugBatch = 0;
	InfluenceDataType debugType = InfluenceDataType::UNITS_INFLUENCE_PER_PLAYER;
};
