#pragma once

#include <vector>
#include "simulation/env/bucket/BucketIterator.h"
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
	Enviroment(Terrain * _terrian);
	~Enviroment();
	static float getSqDistance(Vector3* unitPosition, Vector3* otherPosition);
	std::vector<Unit *>* getNeighbours(Unit* unit, double radius);
	std::vector<Unit *>* getNeighboursFromTeam(Unit* unit, double radius, int team, OperatorType operatorType);
//	std::vector<Physical *>* getBuildings(Unit* unit, double radius);

	std::vector<Unit *>* getNeighbours(Unit* unit, Grid* bucketGrid, double radius);

	//std::vector<Physical*>* getResources(Unit* unit, double radius);

	void update(std::vector<Unit*>* units);
	void update(std::vector<Building*>* buildings);
	void update(std::vector<ResourceEntity*>* resources);

	Vector3* validatePosition(Vector3 * position);

	std::vector<Physical *>* getNeighbours(std::pair<Vector3*, Vector3*>* pair);
	std::vector<Physical *>* getBuildings(std::pair<Vector3*, Vector3*>* pair);
	double getGroundHeightAt(double x, double z);
	float getGroundHeightPercent(float y, float x, float div);
	bool validateStatic(const IntVector2& size, Vector3* pos);
	Vector3* getValidPosition(const IntVector2& size, Vector3* pos);
	IntVector2 getBucketCords(const IntVector2& size, Vector3* pos);
	void testFind(IntVector2 &startV, IntVector2 &goalV);
	void prepareGridToFind();
	content_info* getContentInfo(Vector2& from, Vector2& to, bool checks[], int activePlayer);
	Vector3 getValidPosForCamera(float percentX, float percentY, const Vector3& pos, float min);
private:
	MainGrid* mainGrid;
	Grid* teamUnitGrid[MAX_PLAYERS];
	Grid* obstacleGrid;
	Grid* resourceGrid;

	Terrain * terrian;

	std::vector<Unit*>* neights;//TODO tu bedzie trzeba tablica jesli beda watki
	std::vector<Unit*>* neights2;
	std::vector<Unit*>* empty;

};
