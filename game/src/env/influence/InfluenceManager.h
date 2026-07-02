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
class Building;
class ResourceEntity;
class InfluenceMap;
class VisibilityManager;
class Unit;

namespace Urho3D {
	class Terrain;
}

inline constexpr int AI_MAP_COUNT = 12;
inline constexpr int AI_ARMY_MAP_COUNT = 6;
inline constexpr int CENTER_TYPE_COUNT = 4;

class InfluenceManager {
public:
	explicit InfluenceManager(unsigned char numberOfPlayers, float mapSize, Urho3D::Terrain* terrain);
	~InfluenceManager();

	// --- Update (called from game loop) ---
	void updateUnits(std::vector<Unit*>* units) const;
	void updateBuildings(const std::vector<Building*>* buildings) const;

	void updateQuadOther() const;
	void updateWithHistory() const;
	void updateVisibility(std::vector<Building*>* buildings, std::vector<Unit*>* units,
	                      std::vector<ResourceEntity*>* resources) const;
	void resetHistoryThresholds() const;

	// --- Events (called during simulation) ---
	void addCollect(Unit* unit, short resId, float value) const;
	void addAttack(unsigned char player, const Urho3D::Vector2& position, float value) const;

	// --- AI queries ---
	std::optional<Urho3D::Vector2> getCenterOf(CenterType id, unsigned char player) const;
	const std::vector<unsigned>& getAreas(std::span<const float> result, unsigned char player) const;
	std::vector<unsigned> getAreasResBonus(unsigned char resId, unsigned char player) const;
	// Returns candidate area centers (world positions) ordered best-first. Influence-map
	// cell indexes are intentionally NOT exposed; the reference is valid until the next
	// getArea* call (reuses an internal buffer).
	const std::vector<Urho3D::Vector2>& getBestVisibleAreas(std::span<const float> result, unsigned char player);

	// --- Utility ---
	bool isVisible(unsigned char player, const Urho3D::Vector2& pos) const;
	float getVisibilityScore(unsigned char player) const;
	Urho3D::Vector2 getCenter(int index) const;
	int getIndex(const Urho3D::Vector2& position) const;
	float getFieldSize() const;

	// --- Debug / UI ---
	void draw(InfluenceDataType type, unsigned char index);
	void drawAll() const;
	void nextVisibilityType() const;
	content_info* getContentInfo(const Urho3D::Vector2& center, CellState state, int additionalInfos, bool* checks,
	                             int activePlayer) const;

private:
	// --- Private helpers ---
	const std::vector<unsigned>& getBestVisibleIndexes(std::span<InfluenceMap*> maps, std::span<const float> result,
	                                unsigned char player) const;

	const std::vector<Urho3D::Vector2>& centersFromIndexes(const std::vector<unsigned>& indexes) const;
	int getIndexInInfluence(Unit* unit) const;

	// --- Per-player influence maps ---
	std::vector<InfluenceMap*> buildingsInfluencePerPlayer;
	std::vector<InfluenceMap*> unitsInfluencePerPlayer;
	std::vector<InfluenceMap*> attackSpeed;
	std::vector<InfluenceMap*> gatherSpeed[RESOURCES_SIZE];
	std::vector<InfluenceMap*> resNotInBonus[RESOURCES_SIZE];
	std::vector<InfluenceMap*> resNotInBonusAny;
	std::vector<InfluenceMap*> economyInfluence;
	std::vector<InfluenceMap*> armyInfluence;

	// --- AI map views (non-owning, per player) ---
	std::vector<std::array<InfluenceMap*, AI_MAP_COUNT>> mapsForAiPerPlayer;
	std::vector<std::array<InfluenceMap*, AI_ARMY_MAP_COUNT>> mapsForAiArmyPerPlayer;
	std::vector<std::array<InfluenceMap*, RESOURCES_SIZE>> mapsGatherSpeedPerPlayer;
	std::vector<std::array<InfluenceMap*, RESOURCES_SIZE>> mapsResNotInBonusPerPlayer;
	std::vector<std::array<InfluenceMap*, CENTER_TYPE_COUNT>> mapsForCentersPerPlayer;

	// --- Infrastructure ---
	GridCalculator* calculator;
	VisibilityManager* visibilityManager;
	float* sharedTemplateV;

	// --- Scratch buffers ---
	mutable content_info* ci;
	mutable unsigned int arraySize;
	mutable float* errorsSum; // [arraySize]
	mutable std::vector<unsigned> tempIndexes;
	mutable std::vector<Urho3D::Vector2> tempCenters;

	// --- Debug state ---
	short currentDebugBatch = 0;
	InfluenceDataType debugType = InfluenceDataType::UNITS_INFLUENCE_PER_PLAYER;
};
