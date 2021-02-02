#pragma once

#include <Urho3D/Math/Vector2.h>
#include <Urho3D/Math/Vector3.h>
#include "bucket/Grid.h"
#include "bucket/MainGrid.h"
#include "debug/EnvironmentDebugMode.h"
#include "influence/InfluenceManager.h"
#include "objects/unit/state/UnitState.h"
#include "utils/defines.h"

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
	explicit Environment(Urho3D::Terrain* terrian);
	~Environment();

	std::vector<Physical*>* getNeighboursFromTeamEq(Physical* physical, float radius, int team);
	std::vector<Physical*>* getNeighboursFromTeamNotEq(Physical* physical, float radius, int team);

	std::vector<Physical*>* getNeighboursWithCache(Unit* unit, float radius);
	const std::vector<Physical*>* getNeighboursSimilarAs(Physical* clicked) const;

	std::vector<Physical*>* getResources(Physical* physical, float radius);
	std::vector<Physical*>* getResources(Urho3D::Vector3& center, float radius, int id);

	void updateInfluenceUnits1(std::vector<Unit*>* units) const;
	void updateInfluenceUnits2(std::vector<Unit*>* units) const;

	void updateInfluenceResources(std::vector<ResourceEntity*>* resources) const;

	void updateInfluenceOther(std::vector<Building*>* buildings) const;

	void invalidateCaches();
	void update(Unit* unit) const;

	void addNew(const std::vector<Unit*>& units);
	void addNew(Building* building) const;
	void addNew(ResourceEntity* resource) const;

	void removeFromGrids(const std::vector<Unit*>& units) const;
	void removeFromGrids(const std::vector<Building*>& buildingsToDispose,
	                     const std::vector<ResourceEntity*>& resourceToDispose) const;

	void updateAll(std::vector<Building*>* buildings) const;

	Urho3D::Vector2 repulseObstacle(Unit* unit) const;
	std::optional<Urho3D::Vector2> validatePosition(Urho3D::Vector3& position) const;

	const std::vector<Physical*>* getNeighbours(std::pair<Urho3D::Vector3*, Urho3D::Vector3*>& pair, char player);

	float getGroundHeightAt(float x, float z) const;
	float getGroundHeightAt(Urho3D::Vector3& pos) const;
	Urho3D::Vector3 getPosWithHeightAt(float x, float z) const;
	Urho3D::Vector3 getPosWithHeightAt(int index) const;
	float getGroundHeightPercent(float y, float x, float div) const;
	bool validateStatic(const Urho3D::IntVector2& size, Urho3D::Vector2& pos) const;
	bool validateStatic(const Urho3D::IntVector2& size, const Urho3D::IntVector2 bucketCords) const;

	Urho3D::Vector2 getValidPosition(const Urho3D::IntVector2& size, const Urho3D::Vector2& pos) const;
	Urho3D::Vector2 getValidPosition(const Urho3D::IntVector2& size, const Urho3D::IntVector2& bucketCords) const;

	std::vector<int>* findPath(int startIdx, Urho3D::Vector2& aim) const;
	std::vector<int>* findPath(int startIdx, int endIdx) const;
	std::vector<int>* findPath(Urho3D::Vector3& from, Urho3D::Vector2& aim) const;

	void prepareGridToFind() const;
	content_info* getContentInfo(Urho3D::Vector2 centerPercent, bool checks[], int activePlayer);
	float getPositionFromPercent(float value) const;
	Urho3D::Vector3 getValidPosForCamera(float percentX, float percentY, const Urho3D::Vector3& pos, float min) const;

	Urho3D::Vector2 getCenter(int index) const;
	Urho3D::Vector2 getCenter(short x, short z) const;

	Urho3D::Vector2 getPositionInBucket(Unit* unit);

	void invalidatePathCache() const;

	int getIndex(Urho3D::Vector2& pos) const { return mainGrid.getIndex(pos); }
	int getIndex(short x, short z) const { return mainGrid.getIndex(x, z); }
	Urho3D::IntVector2 getCords(int index) const { return mainGrid.getCords(index); }
	Urho3D::IntVector2 getCords(const Urho3D::Vector2& pos) { return mainGrid.getCords(pos); }

	bool cellInState(int index, CellState state) const;
	void updateCell(int index, char val, CellState cellState) const;

	unsigned char getRevertCloseIndex(int center, int gridIndex) const;

	bool isInLocalArea(int getMainCell, int aimIndex) const;
	int closestPassableCell(int posIndex) const;

	void drawDebug(EnvironmentDebugMode environmentDebugMode, char index);

	const std::vector<short>& getCloseIndexs(int center) const;
	const std::vector<unsigned char>& getCloseTabIndexes(int center) const;
	std::array<float, 5>& getInfluenceDataAt(char player, const Urho3D::Vector2& pos);

	std::optional<Urho3D::Vector2> getPosToCreate(db_building* building, char player, const std::span<float> result);
	std::vector<Urho3D::Vector2> getAreas(char player, const std::span<float>, int min);
	void addCollect(Unit* unit, float value);
	void addAttack(Unit* unit, float value);
	void drawInfluence();
	bool cellIsPassable(int index) const;
	bool cellIsCollectable(int index) const;
	Urho3D::Vector2 getCenterOf(CenterType id, char player);

private:
	std::vector<Physical*>* getNeighbours(Physical* physical, Grid& bucketGrid, float radius) const;
	std::vector<Physical*>* getNeighbours(Urho3D::Vector3& center, Grid& bucketGrid, float radius, int id) const;
	void addIfInRange(Physical* physical, const Urho3D::Vector3& center, float sqRadius, Physical* neight) const;
	MainGrid mainGrid;
	Grid resourceGrid, buildingGrid;
	Grid teamUnitGrid[MAX_PLAYERS];
	InfluenceManager influenceManager;
	std::array<Grid*, 3> grids = {&mainGrid, &buildingGrid, &resourceGrid};
	Urho3D::Terrain* terrain;

	std::vector<Physical*> *neights, *neights2; //TODO tu bedzie trzeba tablica jesli beda watki
};
