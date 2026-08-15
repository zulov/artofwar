#pragma once

#include <array>
#include <functional>
#include <optional>
#include <span>
#include <unordered_set>
#include <utility>
#include <vector>
#include <Urho3D/Math/Vector2.h>
#include <Urho3D/Math/Vector3.h>

#include "GridCalculator.h"
#include "bucket/Grid.h"
#include "bucket/MainGrid.h"
#include "bucket/StaticGrid.h"
#include "debug/EnvironmentDebugMode.h"
#include "influence/InfluenceManager.h"
#include "objects/ObjectEnums.h"

struct MouseHeld;
enum class CenterType:char;

namespace Urho3D {
	class Terrain;
}

struct content_info;
class ResourceEntity;
class Unit;
class Building;

class Environment {
public:
	explicit Environment(Urho3D::Terrain* terrain, unsigned short mainMapResolution);
	~Environment();

	const std::vector<Physical*>& getNeighboursFromSparseSamePlayer(const Physical* source,
	                                                               const Urho3D::Vector2& center, float radius,
	                                                               char playerId);
	const std::vector<Physical*>& getNeighboursFromTeamNotEq(const Physical* source, const Urho3D::Vector2& center,
	                                                        char playerId, float radius);

	const std::vector<Physical*>& getNeighboursWithCache(const Physical* source, const Urho3D::Vector2& center,
	                                                    int gridIndex, float radius);
	const std::vector<Physical*>& getNeighboursSimilarAs(ObjectType objectType, const Urho3D::Vector2& center,
	                                                     unsigned short databaseId, char playerId) const;

	const std::vector<Physical*>& getResources(const Urho3D::Vector2& center, int resourceId, int resourceLevel);
	const std::vector<Physical*>& getResources(const Urho3D::Vector2& center, float radius);
	std::vector<int> getUniqueResourceIndexesInRange(const std::unordered_set<int>& centerIndexes, float radius) const;
	const std::vector<Physical*>& getResourcesAt(int cellIndex) const;

	int getResourceLevelCount() const { return resourceStaticGrid.levelCount(); }

	const std::vector<Physical*>& getBuildingsFromTeamNotEq(const Physical* source, const Urho3D::Vector2& center,
	                                                       char teamId, int buildingId,
	                                                       float radius);

	void updateInfluenceUnits(std::span<Unit* const> units) const;

	void updateInfluenceBuildings(std::span<Building* const> buildings) const;
	void updateVisibility(std::span<Building* const> buildings, std::span<Unit* const> units,
	                      std::span<ResourceEntity* const> resources) const;

	void invalidateCaches();
	void update(Unit* unit) const;

	void addNew(std::span<Unit* const> units);
	void addNew(Building* building, bool bulkAdd);
	void addNew(ResourceEntity* resource, bool bulkAdd);

	void removeFromGrids(std::span<Unit* const> units);
	void removeFromGrids(std::span<Building* const> buildingsToDispose,
	                     std::span<ResourceEntity* const> resourcesToDispose);

	Urho3D::Vector2 repulseObstacle(Unit* unit);
	std::optional<Urho3D::Vector2> validatePosition(int index, const Urho3D::Vector2& position) const;

	const std::vector<Physical*>& getNeighbours(MouseHeld& held, char playerId);

	float getGroundHeightAt(float x, float z) const;
	float getGroundHeightAt(const Urho3D::Vector2& pos) const;
	Urho3D::Vector3 getPosWithHeightAt(float x, float z) const;

	Urho3D::Vector3 getPosWithHeightAt(int index) const;
	float getGroundHeightPercentScaled(float x, float z, float div) const;

	bool validateStatic(const Urho3D::UCharVector2& size, const Urho3D::Vector2& position, bool isBuilding) const;
	bool validateStatic(const Urho3D::UCharVector2& size, const Urho3D::UShortVector2& bucketCords, bool isBuilding) const;
	bool validateStatic(const Urho3D::UCharVector2& size, int index, bool isBuilding) const;
	std::optional<Urho3D::Vector2> tryGetValidPosition(const Urho3D::UCharVector2& size,
	                                                  const Urho3D::UShortVector2& bucketCords,
	                                                  bool isBuilding) const;

	Urho3D::Vector3 getValidPosition(const Urho3D::UCharVector2& size, const Urho3D::Vector2& pos) const;

	const std::vector<int>* findPath(int startIdx, const Urho3D::Vector2& aim);
	const std::vector<int>* findPath(int startIdx, std::span<const int> endIndexes);
	const std::vector<int>* findPath(int startIdx, int endIdx);

	void prepareGridToFind() const;
	content_info* getContentInfo(Urho3D::Vector2 centerPercent, bool checks[], int activePlayer);

	Urho3D::Vector2 getPosFromPercent(float x, float z) const;
	Physical* closestPhysical(int startIdx, const std::vector<Physical*>& things,
	                          const std::function<bool(Physical*)>& condition, bool closeEnough);
	Physical* closestPhysicalSimple(const Urho3D::Vector2& center, const std::vector<Physical*>& things,
	                               float range) const;

	Urho3D::Vector2 getCenter(int index) const;

	int getIndex(const Urho3D::Vector2& position) const { return calculator->indexFromPosition(position); }
	int getIndex(short x, short z) const { return calculator->getIndex(x, z); }
	Urho3D::UShortVector2 getCords(int index) const { return calculator->getCords(index); }

	Urho3D::UShortVector2 getCords(const Urho3D::Vector2& pos) const { return calculator->getCords(pos); }

	bool cellInState(int index, CellState state) const;
	void incCell(int index, CellState cellState) const;
	void decCell(int index) const;

	unsigned char getRevertCloseIndex(int center, int gridIndex) const;

	bool isInLocalArea(int getMainCell, int aimIndex) const;
	bool isInLocal1and2Area(int getMainCell, int aimIndex) const;
	int closestPassableCell(int posIndex) const;

	void drawDebug(EnvironmentDebugMode environmentDebugMode, char index);

	const std::vector<short>& getCloseIndexs(int center) const;

	std::optional<Urho3D::Vector2> getPosToCreate(std::span<const float> result,
	                                              const Urho3D::UCharVector2& buildingSize, unsigned char player);
	void addCollect(Unit* unit, short resId, float value) const;
	void addAttack(char player, const Urho3D::Vector2& position, float value) const;
	void drawInfluence();
	bool cellIsPassable(int index) const;
	bool cellIsCollectable(int index) const;
	bool cellIsAttackable(int index) const;
	std::optional<Urho3D::Vector2> getCenterOf(CenterType type, unsigned char player) const;
	float getDiffOfCenters(CenterType type1, char id1, CenterType type2, char id2, float defaultVal) const;
	bool anyCloseEnough(std::span<const int> indexes, int center, float distThreshold) const;
	unsigned short getResolution() const { return calculator->getResolution(); }
	bool isVisible(char player, const Urho3D::Vector2& pos) const;
	float getVisibilityScore(char player) const;

	std::vector<int> getIndexesInRange(const Urho3D::Vector2& center, float range) const;
	std::vector<int> getIndexesInRange(int index, float range) const;
	Urho3D::Terrain* getTerrain() const { return terrain; }
	void setTerrainShaderParam(const Urho3D::String& name, const Urho3D::Variant& value) const;
	void flipTerrainShaderParam(const Urho3D::String& name) const;
	void nextVisibilityType() const;
	void reAddBonuses(std::span<Building* const> resourceBuildings,
	                  std::span<ResourceEntity* const> resources) const;

	const std::vector<std::pair<unsigned char, short>>& getCloseTabIndexesWithValue(int center) const {
		return mainGrid.getCloseTabIndexesWithValue(center);
	}

	void refreshAllStatic(std::span<ResourceEntity* const> resources, std::span<Building* const> buildings);
	short getOccupationLevel(int index) const;
private:
	struct DebugTerrainCornerCache {
		std::vector<std::array<Urho3D::Vector3, 4>> cells;
	};

	Urho3D::Vector3 getValidPosition(const Urho3D::UCharVector2& size,
	                                const Urho3D::UShortVector2& bucketCords) const;
	std::optional<Urho3D::Vector2> getPosFromIndexes(const Urho3D::UCharVector2& buildingSize, unsigned char player,
	                                                 std::span<const unsigned> indexes);

	const std::vector<Physical*>& getNeighbours(const Physical* source, const Urho3D::Vector2& center, Grid& grid,
	                                      float radius,
	                                      const std::function<bool(const Physical*)>& condition = {}) const;

	void addIfInRange(const Physical* source, const Urho3D::Vector2& center, Physical* neighbor, float squaredRadius,
	                  const std::function<bool(const Physical*)>& condition) const;
	float getPosFromPercent(float value) const;
	float mapSize;
	MainGrid mainGrid;
	Grid buildingGrid;
	StaticGrid resourceStaticGrid;
	Grid sparseUnitGrid;
	Urho3D::Terrain* terrain;
	InfluenceManager influenceManager;
	Grid* grids[3] = {&mainGrid, &buildingGrid, &resourceStaticGrid};
	GridCalculator* calculator;

	mutable std::vector<Physical*> neighbors;
};
