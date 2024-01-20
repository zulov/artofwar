#pragma once

#include <functional>
#include <Urho3D/Math/Vector2.h>
#include <Urho3D/Math/Vector3.h>

#include "GridCalculator.h"
#include "bucket/Grid.h"
#include "bucket/MainGrid.h"
#include "bucket/StaticGrid.h"
#include "debug/EnvironmentDebugMode.h"
#include "influence/InfluenceManager.h"

enum class CenterType:char;
struct db_building;

namespace Urho3D {
	class Terrain;
}

struct content_info;
class ResourceEntity;
class Unit;
class Building;
enum class OperatorType : char;

class Environment {
public:
	explicit Environment(Urho3D::Terrain* terrain, unsigned short mainMapResolution);
	~Environment();

	std::vector<Physical*>* getNeighboursFromSparseSamePlayer(Physical* physical, float radius, char player);
	std::vector<Physical*>* getNeighboursFromTeamNotEq(Physical* physical, float radius);

	std::vector<Physical*>* getNeighboursWithCache(Unit* unit, float radius);
	const std::vector<Physical*>* getNeighboursSimilarAs(Physical* clicked) const;

	//std::vector<Physical*>* getResources(Physical* physical, float radius);
	std::vector<Physical*>* getResources(const Urho3D::Vector3& center, int id, float radius, float prevRadius);
	std::vector<Physical*>* getResources(const Urho3D::Vector3& center, int id, float radius);

	std::vector<Physical*>* getBuildingsFromTeamNotEq(Physical* physical, int id, float radius);

	void updateInfluenceUnits1(std::vector<Unit*>* units);
	void updateInfluenceUnits2(std::vector<Unit*>* units);

	void updateInfluenceResources(std::vector<ResourceEntity*>* resources) const;

	void updateInfluenceOther(std::vector<Building*>* buildings, std::vector<Unit*>* units) const;
	void updateQuadOther() const;
	void updateVisibility(std::vector<Building*>* buildings, std::vector<Unit*>* units,
	                      std::vector<ResourceEntity*>* resources) const;
	void updateInfluenceHistoryReset() const;
	void invalidateCaches();
	void update(Unit* unit) const;

	void addNew(const std::vector<Unit*>& units);
	void addNew(Building* building, bool bulkAdd);
	void addNew(ResourceEntity* resource, bool bulkAdd);

	void removeFromGrids(const std::vector<Unit*>& units) const;
	void removeFromGrids(const std::vector<Building*>& buildingsToDispose,
	                     const std::vector<ResourceEntity*>& resourceToDispose);

	Urho3D::Vector2 repulseObstacle(Unit* unit) const;
	std::optional<Urho3D::Vector2> validatePosition(int index, const Urho3D::Vector3& position) const;

	const std::vector<Physical*>* getNeighbours(std::pair<Urho3D::Vector3*, Urho3D::Vector3*>& pair, char player);

	float getGroundHeightAt(float x, float z) const;
	float getGroundHeightAt(const Urho3D::Vector3& pos) const;
	Urho3D::Vector3 getPosWithHeightAt(float x, float z) const;
	Urho3D::Vector3 getPosWithHeightAt(int index) const;
	float getGroundHeightPercentScaled(float x, float z, float div) const;

	bool validateStatic(const Urho3D::IntVector2& size, Urho3D::Vector2& pos, bool isBuilding) const;
	bool validateStatic(const Urho3D::IntVector2& size, const Urho3D::IntVector2 bucketCords, bool isBuilding) const;

	Urho3D::Vector2 getValidPosition(const Urho3D::IntVector2& size, const Urho3D::Vector2& pos) const;
	Urho3D::Vector2 getValidPosition(const Urho3D::IntVector2& size, const Urho3D::IntVector2& bucketCords) const;

	const std::vector<int>* findPath(int startIdx, const Urho3D::Vector2& aim);
	const std::vector<int>* findPath(int startIdx, const std::vector<int>& endIdxs);
	const std::vector<int>* findPath(int startIdx, int endIdx, int limit);

	void prepareGridToFind() const;
	content_info* getContentInfo(Urho3D::Vector2 centerPercent, bool checks[], int activePlayer);

	Urho3D::Vector2 getPosFromPercent(float x, float z) const;
	Physical* closestPhysical(int startIdx, const std::vector<Physical*>* things,
	                          const std::function<bool(Physical*)>& condition, int limit, bool closeEnough);
	Physical* closestPhysicalSimple(const Physical* physical, const std::vector<Physical*>* things, float range) const;

	Urho3D::Vector2 getCenter(int index) const;

	void invalidatePathCache();

	int getIndex(Urho3D::Vector2& pos) const { return calculator->indexFromPosition(pos); }
	int getIndex(short x, short z) const { return calculator->getIndex(x, z); }
	Urho3D::IntVector2 getCords(int index) const { return calculator->getCords(index); }

	Urho3D::IntVector2 getCords(const Urho3D::Vector2& pos) {
		return calculator->getCords(calculator->indexFromPosition(pos));
	}

	bool cellInState(int index, CellState state) const;
	void updateCell(int index, char val, CellState cellState) const;

	unsigned char getRevertCloseIndex(int center, int gridIndex) const;

	bool isInLocalArea(int getMainCell, int aimIndex) const;
	bool isInLocal1and2Area(int getMainCell, int aimIndex) const;
	int closestPassableCell(int posIndex) const;

	void drawDebug(EnvironmentDebugMode environmentDebugMode, char index);

	const std::vector<short>& getCloseIndexs(int center) const;
	std::array<float, 5>& getInfluenceDataAt(char player, const Urho3D::Vector2& pos);
	std::optional<Urho3D::Vector2> getPosFromIndexes(db_building* building, char player,
	                                                 const std::vector<unsigned>* indexes);

	std::optional<Urho3D::Vector2> getPosToCreate(const std::span<float> result, ParentBuildingType type,
	                                              db_building* building, char player);
	std::optional<Urho3D::Vector2> getPosToCreateResBonus(db_building* building, char player);
	std::vector<Urho3D::Vector2> getAreas(char player, const std::span<float>, int min);
	void addCollect(Unit* unit, short resId, float value);
	void addAttack(char player, const Urho3D::Vector3& position, float value);
	void drawInfluence();
	bool cellIsPassable(int index) const;
	bool cellIsCollectable(int index) const;
	bool cellIsAttackable(int index) const;
	std::optional<Urho3D::Vector2> getCenterOf(CenterType type, char player);
	float getDiffOfCenters(CenterType type1, char id1, CenterType type2, char id2, float defaultVal);
	bool anyCloseEnough(std::vector<int> const& indexes, int center, float distThreshold) const;
	unsigned short getResolution() const { return calculator->getResolution(); }
	bool isVisible(char player, const Urho3D::Vector2& pos) const;
	float getVisibilityScore(char player) const;

	std::vector<int> getIndexesInRange(const Urho3D::Vector3& center, float range) const;
	Urho3D::Terrain* getTerrain() const { return terrain; }
	void setTerrainShaderParam(const Urho3D::String& name, const Urho3D::Variant& value) const;
	void flipTerrainShaderParam(const Urho3D::String& name) const;
	void nextVisibilityType() const;
	void reAddBonuses(std::vector<Building*>& resBuildings, std::vector<ResourceEntity*>* resources) const;

	void refreshAllStatic(std::vector<int>& indexes);

	const std::vector<std::pair<unsigned char, short>>& getCloseTabIndexesWithValue(int center) const {
		return mainGrid.getCloseTabIndexesWithValue(center);
	}

private:
	std::vector<Physical*>* getNeighbours(Physical* physical, Grid& bucketGrid, float radius,
	                                      const std::function<bool(Physical*)>& condition) const;
	std::vector<Physical*>* getNeighbours(Unit* unit, float radius);
	std::vector<Physical*>* getNeighbours(const Urho3D::Vector3& center, Grid& bucketGrid, int id, float radius,
	                                      float prevRadius) const;
	void addIfInRange(const Physical* physical, Physical* neight, const float sqRadius,
	                  const std::function<bool(Physical*)>& condition) const;
	void addIfInRange(const Physical* physical, Physical* neight, const float sqRadius) const;
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

	std::vector<Physical*>* neights;
};
