#pragma once

#include "ContentInfo.h"
#include "bucket/MainGrid.h"
#include "defines.h"
#include <Urho3D/Graphics/Terrain.h>
#include <vector>


class ResourceEntity;
class Unit;
class Building;
enum class OperatorType : char;

class Enviroment
{
public:
	Enviroment(Terrain* _terrian);
	~Enviroment();

	std::vector<Physical *>* getNeighbours(Physical* physical, float radius);
	std::vector<Physical *>* getNeighboursFromTeam(Physical* physical, float radius, int team,
	                                               OperatorType operatorType);
	std::vector<Physical *>* getNeighbours(Physical* physical, Grid& bucketGrid, float radius) const;

	std::vector<Physical*>* getResources(Physical* physical, float radius);

	void update(std::vector<Unit*>* units) const;
	void update(std::vector<Building*>* buildings);
	void update(std::vector<ResourceEntity*>* resources);

	Vector2 repulseObstacle(Unit* unit);
	Vector2* validatePosition(Vector3* position);

	std::vector<Physical *>* getNeighbours(std::pair<Vector3*, Vector3*>& pair);
	std::vector<Physical *>* getBuildings(std::pair<Vector3*, Vector3*>& pair);

	float getGroundHeightAt(float x, float z) const;
	float getGroundHeightPercent(float y, float x, float div) const;
	bool validateStatic(const IntVector2& size, Vector2& pos);
	Vector2 getValidPosition(const IntVector2& size, const Vector2& pos);
	IntVector2 getBucketCords(const IntVector2& size, const Vector2& pos) const;
	std::vector<int>* findPath(int startIdx, Vector2& aim);

	void prepareGridToFind();
	content_info* getContentInfo(Vector2 from, Vector2 to, bool checks[], int activePlayer);
	Vector3 getValidPosForCamera(float percentX, float percentY, const Vector3& pos, float min) const;
	Vector2 getValidPosition(const IntVector2& size, const IntVector2& bucketCords);
	Vector2& getCenter(int index) const;
	Vector2& getCenter(short x, short z);
	void invalidateCache();

	int getIndex(Vector2& pos) const;
	int getIndex(short x, short z) const;
	CellState getType(int index) const { return mainGrid.getType(index); }
	char getCurrentSize(int index) const { return mainGrid.getCurrentSize(index); }
	bool cellInState(int index, std::vector<CellState> cellStates) const;
	void updateCell(int index, char val, CellState cellState);
	bool belowCellLimit(int index);
private:
	MainGrid mainGrid;
	Grid resourceGrid;
	Grid buildingGrid;
	Grid teamUnitGrid[MAX_PLAYERS];

	Terrain* terrian;

	std::vector<Physical*>* neights; //TODO tu bedzie trzeba tablica jesli beda watki
	std::vector<Physical*>* neights2;
};
