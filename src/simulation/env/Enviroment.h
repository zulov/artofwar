#pragma once

#include <vector>
#include "simulation/env/bucket/Grid.h"
#include "objects/building/Building.h"
#include "defines.h"
#include "OperatorType.h"
#include "objects/resource/ResourceEntity.h"
#include "objects/unit/Unit.h"
#include "objects/Physical.h"
#include "bucket/MainGrid.h"
#include <Urho3D/Graphics/Terrain.h>
#include "ContentInfo.h"


class Enviroment
{
public:
	Enviroment(Terrain* _terrian);
	~Enviroment();

	std::vector<Unit *>* getNeighbours(Unit* unit, float radius);
	std::vector<Unit *>* getNeighboursFromTeam(Unit* unit, float radius, int team, OperatorType operatorType);

	std::vector<Unit *>* getNeighbours(Unit* unit, Grid& grid, float radius);

	void update(std::vector<Unit*>* units);
	void update(std::vector<Building*>* buildings);
	void update(std::vector<ResourceEntity*>* resources);

	Vector2 repulseObstacle(Unit* unit);
	Vector2* validatePosition(Vector3* position);

	std::vector<Physical *>* getNeighbours(std::pair<Vector3*, Vector3*>& pair);
	std::vector<Physical *>* getBuildings(std::pair<Vector3*, Vector3*>& pair);
	float getGroundHeightAt(float x, float z);
	float getGroundHeightPercent(float y, float x, float div);
	bool validateStatic(const IntVector2& size, Vector2& pos);
	Vector2 getValidPosition(const IntVector2& size, Vector2& pos);
	IntVector2 getBucketCords(const IntVector2& size, Vector2& pos);
	std::vector<int>* findPath(int startIdx, Vector2& aim);

	void prepareGridToFind();
	content_info* getContentInfo(Vector2 from, Vector2 to, bool checks[], int activePlayer);
	Vector3 getValidPosForCamera(float percentX, float percentY, const Vector3& pos, float min);
	Vector2 getValidPosition(const IntVector2& size, const IntVector2& bucketCords);
	Vector2& getCenter(int index);
	void invalidateCache();

	int getIndex(Vector2& pos);
private:
	MainGrid mainGrid;
	Grid resourceGrid;
	Grid obstacleGrid;
	Grid teamUnitGrid[MAX_PLAYERS];

	Terrain* terrian;

	std::vector<Unit*>* neights; //TODO tu bedzie trzeba tablica jesli beda watki
	std::vector<Unit*>* neights2;
};
