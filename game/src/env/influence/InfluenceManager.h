#pragma once

#include <span>
#include <array>
#include <optional>
#include <vector>
#include <Urho3D/Math/Vector3.h>

#include "debug/EnvironmentDebugMode.h"
#include "objects/resource/ResourceType.h"

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

inline constexpr int AI_MAP_COUNT = 11;
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
	// --- Utility ---
	bool isVisible(unsigned char player, const Urho3D::Vector2& pos) const;
	float getVisibilityScore(unsigned char player) const;
	Urho3D::Vector2 getCenter(int index) const;
	int getIndex(const Urho3D::Vector2& position) const;
	float getFieldSize() const;

	// --- Debug / UI ---
	void draw(EnvironmentDebugMode mode, unsigned char index);
	void drawAll() const;
	void nextVisibilityType() const;
	content_info* getContentInfo(const Urho3D::Vector2& center, CellState state, int additionalInfos, bool* checks,
	                             int activePlayer) const;

private:
	// --- Private helpers ---
	const std::vector<unsigned>& getBestVisibleIndexes(std::span<InfluenceMap*> maps, std::span<const float> result,
	                                unsigned char player) const;

	int getIndexInInfluence(Unit* unit) const;

	// --- Per-player influence maps ---
	std::vector<InfluenceMap*> buildingPresence;
	std::vector<InfluenceMap*> unitPresence;
	std::vector<InfluenceMap*> armyPresence;
	std::vector<InfluenceMap*> gatheringActivityByResource[RESOURCES_SIZE];
	std::vector<InfluenceMap*> attackActivity;
	std::vector<InfluenceMap*> unboostedResourceByResource[RESOURCES_SIZE];
	std::vector<InfluenceMap*> unboostedResource;
	std::vector<InfluenceMap*> economicActivity;

	// --- AI map views (non-owning, per player) ---
	std::vector<std::array<InfluenceMap*, AI_MAP_COUNT>> buildPlacementByPlayer;
	std::vector<std::array<InfluenceMap*, RESOURCES_SIZE>> unboostedResourceByPlayer;
	std::vector<std::array<InfluenceMap*, CENTER_TYPE_COUNT>> centerSourceByPlayer;

	// --- Infrastructure ---
	GridCalculator* calculator;
	VisibilityManager* visibilityManager;

	// --- Scratch buffers ---
	mutable content_info* ci;
	mutable unsigned int arraySize;
	mutable std::vector<float> errorsSum; // [arraySize]
	mutable std::vector<unsigned> tempIndexes;

	// --- Debug state ---
	short currentDebugBatch = 0;
};
