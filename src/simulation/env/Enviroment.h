#pragma once

#include "ContentInfo.h"
#include "bucket/MainGrid.h"
#include "defines.h"
#include "objects/unit/state/UnitState.h"
#include <Urho3D/Graphics/Terrain.h>
#include <vector>


class ResourceEntity;
class Unit;
class Building;
enum class OperatorType : char;

class Enviroment
{
public:
	explicit Enviroment(Urho3D::Terrain* _terrian);
	~Enviroment();

	std::vector<Physical *>* getNeighbours(Physical* physical, float radius);
	std::vector<Physical *>* getNeighboursFromTeam(Physical* physical, float radius, int team,
	                                               OperatorType operatorType);
	std::vector<Physical *>* getNeighbours(Physical* physical, Grid& bucketGrid, float radius) const;

	std::vector<Physical*>* getResources(Physical* physical, float radius);

	void update(std::vector<Unit*>* units) const;
	void update(std::vector<Building*>* buildings);
	void update(std::vector<ResourceEntity*>* resources);

	Urho3D::Vector2 repulseObstacle(Unit* unit);
	Urho3D::Vector2* validatePosition(Urho3D::Vector3* position);

	std::vector<Physical *>* getNeighbours(std::pair<Urho3D::Vector3*, Urho3D::Vector3*>& pair);

	float getGroundHeightAt(float x, float z) const;
	float getGroundHeightPercent(float y, float x, float div) const;
	bool validateStatic(const Urho3D::IntVector2& size, Urho3D::Vector2& pos);
	Urho3D::Vector2 getValidPosition(const Urho3D::IntVector2& size, const Urho3D::Vector2& pos);
	Urho3D::IntVector2 getBucketCords(const Urho3D::IntVector2& size, const Urho3D::Vector2& pos) const;
	std::vector<int>* findPath(int startIdx, Urho3D::Vector2& aim);

	void prepareGridToFind();
	content_info* getContentInfo(Urho3D::Vector2 from, Urho3D::Vector2 to, bool checks[], int activePlayer);
	Urho3D::Vector3 getValidPosForCamera(float percentX, float percentY, const Urho3D::Vector3& pos, float min) const;
	Urho3D::Vector2 getValidPosition(const Urho3D::IntVector2& size, const Urho3D::IntVector2& bucketCords);
	Urho3D::Vector2& getCenter(int index) const;
	Urho3D::Vector2& getCenter(short x, short z);

	Urho3D::Vector2 getPositionInBucket(int index, char max, char i);

	void invalidateCache();

	int getIndex(Urho3D::Vector2& pos) const { return mainGrid.indexFromPosition(pos); }
	int getIndex(short x, short z) const { return mainGrid.getIndex(x, z); }
	Urho3D::IntVector2 getCords(int index) const { return mainGrid.getCords(index); }
	CellState getType(int index) const { return mainGrid.getType(index); }
	char getCurrentSize(int index) const { return mainGrid.getCurrentSize(index); }
	bool cellInState(int index, std::vector<CellState> cellStates) const;
	void updateCell(int index, char val, CellState cellState);
	bool belowCellLimit(int index) const;
	char getNumberInState(int index, UnitState state) const;
	char getOrdinarInState(Unit* unit, UnitState state) const;
	void removeFromGrids(const std::vector<Physical*>& toDispose);
private:
	MainGrid mainGrid;
	Grid resourceGrid, buildingGrid;
	Grid teamUnitGrid[MAX_PLAYERS];

	Urho3D::Terrain* terrian;

	std::vector<Physical*> *neights, *neights2, *empty; //TODO tu bedzie trzeba tablica jesli beda watki
};
