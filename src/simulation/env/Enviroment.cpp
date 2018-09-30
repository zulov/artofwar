#include "Enviroment.h"
#include "ObjectEnums.h"
#include "OperatorType.h"
#include "objects/building/Building.h"
#include "objects/resource/ResourceEntity.h"
#include "objects/unit/Unit.h"
#include <chrono>
#include <simulation/env/bucket/BucketIterator.h>
#include <unordered_set>

#define BUCKET_GRID_RESOLUTION 512
#define BUCKET_GRID_SIZE 1024

#define BUCKET_GRID_RESOLUTION_BUILD 128
#define BUCKET_GRID_RESOLUTION_ENEMY 64
#define BUCKET_GRID_RESOLUTION_RESOURCE 128

#define BUCKET_GRID_SIZE_BUILD 1024
#define BUCKET_GRID_SIZE_ENEMY 1024
#define BUCKET_GRID_SIZE_RESOURCE 1024

Enviroment::Enviroment(Urho3D::Terrain* _terrian):
	mainGrid(BUCKET_GRID_RESOLUTION, BUCKET_GRID_SIZE),
	resourceGrid(BUCKET_GRID_RESOLUTION_RESOURCE, BUCKET_GRID_SIZE_RESOURCE),
	buildingGrid(BUCKET_GRID_RESOLUTION_BUILD, BUCKET_GRID_SIZE_BUILD),
	teamUnitGrid{
		{BUCKET_GRID_RESOLUTION_ENEMY, BUCKET_GRID_SIZE_ENEMY},
		{BUCKET_GRID_RESOLUTION_ENEMY, BUCKET_GRID_SIZE_ENEMY}
	} {
	neights = new std::vector<Physical *>();
	neights2 = new std::vector<Physical *>();
	empty = new std::vector<Physical *>();
	neights->reserve(DEFAULT_VECTOR_SIZE * 2);
	neights2->reserve(DEFAULT_VECTOR_SIZE * 2);

	terrian = _terrian;
}


Enviroment::~Enviroment() {
	delete neights;
	delete neights2;
}

std::vector<Physical*>* Enviroment::getNeighbours(Physical* physical, const float radius) {
	return getNeighbours(physical, mainGrid, radius);
}

std::vector<Physical*>* Enviroment::getNeighboursFromTeam(Physical* physical, const float radius, const int team,
                                                          const OperatorType operatorType) {
	switch (operatorType) {
	case OperatorType::EQUAL:
		return getNeighbours(physical, teamUnitGrid[team], radius);
	case OperatorType::NOT_EQUAL:
		{
		neights2->clear();
		for (int i = 0; i < MAX_PLAYERS; ++i) {
			if (team != i) {
				auto neightLocal = getNeighbours(physical, teamUnitGrid[i], radius);
				neights2->insert(neights2->end(), neightLocal->begin(), neightLocal->end());
			}
		}
		return neights2;
		}
	}
}

std::vector<Physical *>* Enviroment::getNeighbours(Physical* physical, Grid& bucketGrid, float radius) const {
	neights->clear();

	auto center = physical->getPosition();
	BucketIterator& bucketIterator = bucketGrid.getArrayNeight(physical->getPosition(), radius, 0);
	const float sqSepDistance = radius * radius;

	while (Physical* neight = bucketIterator.next()) {
		if (physical == neight) { continue; }
		const float xDiff = center->x_ - neight->getPosition()->x_;
		const float zDiff = center->z_ - neight->getPosition()->z_;

		if (xDiff * xDiff + zDiff * zDiff < sqSepDistance) {
			neights->push_back(neight);
		}
	}

	return neights;
}

std::vector<Physical*>* Enviroment::getResources(Physical* physical, float radius) {
	return getNeighbours(physical, resourceGrid, radius);
}

void Enviroment::update(std::vector<Unit*>* units) const {
	//TODO to mozna rodzielic na dodawanei u usywanie
	for (auto unit : *units) {
		mainGrid.update(unit);
		teamUnitGrid[unit->getTeam()].update(unit, unit->getTeam());
	}
}

void Enviroment::update(std::vector<Building*>* buildings) {
	for (auto building : *buildings) {
		mainGrid.addStatic(building);
		buildingGrid.update(building);
		mainGrid.updateSurround(building);
	}
}

void Enviroment::update(std::vector<ResourceEntity*>* resources) {
	for (auto resource : *resources) {
		mainGrid.addStatic(resource);
		resourceGrid.update(resource);
		mainGrid.updateSurround(resource);
	}
}

Urho3D::Vector2 Enviroment::repulseObstacle(Unit* unit) {
	return mainGrid.repulseObstacle(unit);
}

Urho3D::Vector2* Enviroment::validatePosition(Urho3D::Vector3* position) {
	return mainGrid.getDirectionFrom(position);
}

std::vector<Physical*>* Enviroment::getNeighbours(std::pair<Urho3D::Vector3*, Urho3D::Vector3*>& pair) {
	std::array<Grid*, 3> grids = {&mainGrid, &buildingGrid, &resourceGrid};
	for (auto grid : grids) {
		auto result = grid->getArrayNeight(pair);
		if (!result->empty()) {
			return result;
		}
	}
	return empty;
}

std::vector<Physical*>* Enviroment::getBuildings(std::pair<Urho3D::Vector3*, Urho3D::Vector3*>& pair) {
	return buildingGrid.getArrayNeight(pair);
}

float Enviroment::getGroundHeightAt(float x, float z) const {
	return terrian->GetHeight(Urho3D::Vector3(x, 0, z));
}

float Enviroment::getGroundHeightPercent(float y, float x, float div) const {
	const float scale = terrian->GetSpacing().y_;
	auto a = Urho3D::Vector3(x * BUCKET_GRID_SIZE - BUCKET_GRID_SIZE * 0.5, 0,
	                         y * BUCKET_GRID_SIZE - BUCKET_GRID_SIZE * 0.5);

	return terrian->GetHeight(a) / scale / div;
}

Urho3D::Vector3 Enviroment::getValidPosForCamera(float percentX, float percentY, const Urho3D::Vector3& pos,
                                                 float min) const {
	auto a = Urho3D::Vector3(percentX * BUCKET_GRID_SIZE - BUCKET_GRID_SIZE * 0.5, pos.y_,
	                         percentY * BUCKET_GRID_SIZE - BUCKET_GRID_SIZE * 0.5);
	const float h = terrian->GetHeight(a);
	if (h + min > pos.y_) {
		a.y_ = h + min;
	}
	return a;
}

bool Enviroment::validateStatic(const Urho3D::IntVector2& size, Urho3D::Vector2& pos) {
	return mainGrid.validateAdd(size, pos);
}

Urho3D::Vector2 Enviroment::getValidPosition(const Urho3D::IntVector2& size, const Urho3D::IntVector2& bucketCords) {
	return getValidPosition(size, mainGrid.getCenterAt(bucketCords));
}

Urho3D::Vector2& Enviroment::getCenter(int index) const {
	return mainGrid.getCenter(index);
}

Urho3D::Vector2& Enviroment::getCenter(short x, short z) {
	return mainGrid.getCenter(x, z);
}

Urho3D::Vector2 Enviroment::getPositionInBucket(int index, char max, char i) {
	return mainGrid.getPositionInBucket(index, max, i);
}

void Enviroment::invalidateCache() {
	mainGrid.invalidateCache();
}

bool Enviroment::cellInState(int index, std::vector<CellState> cellStates) const {
	return mainGrid.cellInStates(index, cellStates);
}

void Enviroment::updateCell(int index, char val, CellState cellState) {
	mainGrid.updateCell(index, val, cellState);
}

bool Enviroment::belowCellLimit(int index) const {
	return mainGrid.belowCellLimit(index);
}

char Enviroment::getNumberInState(int index, UnitState state) const {
	return mainGrid.getNumberInState(index, state);
}

char Enviroment::getOrdinarInState(Unit* unit, UnitState state) const {
	return mainGrid.getOrdinarInState(unit, state);
}

void Enviroment::removeFromGrids(const std::vector<Physical*>& toDispose) {
	for (auto dispose : toDispose) {
		switch (dispose->getType()) {
		case ObjectType::BUILDING:
			{
			const auto building = static_cast<Building*>(dispose);
			mainGrid.removeStatic(building);
			buildingGrid.update(dispose);
			mainGrid.updateSurround(building);
			}
			break;
		case ObjectType::RESOURCE:
			{
			const auto resource = static_cast<ResourceEntity*>(dispose);
			mainGrid.removeStatic(resource);
			resourceGrid.update(resource);
			mainGrid.updateSurround(resource);
			}
			break;
		default: ;
		}
	}
}

Urho3D::Vector2 Enviroment::getValidPosition(const Urho3D::IntVector2& size, const Urho3D::Vector2& pos) {
	return mainGrid.getValidPosition(size, pos);
}

Urho3D::IntVector2 Enviroment::getBucketCords(const Urho3D::IntVector2& size, const Urho3D::Vector2& pos) const {
	return mainGrid.getBucketCords(size, pos);
}

std::vector<int>* Enviroment::findPath(int startIdx, Urho3D::Vector2& aim) {
	return mainGrid.findPath(startIdx, aim);
}

void Enviroment::prepareGridToFind() {
	mainGrid.prepareGridToFind();
}

content_info* Enviroment::getContentInfo(Urho3D::Vector2 from, Urho3D::Vector2 to, bool checks[], int activePlayer) {
	from.x_ = from.x_ * BUCKET_GRID_SIZE - BUCKET_GRID_SIZE * 0.5;
	from.y_ = from.y_ * BUCKET_GRID_SIZE - BUCKET_GRID_SIZE * 0.5;

	to.x_ = to.x_ * BUCKET_GRID_SIZE - BUCKET_GRID_SIZE * 0.5;
	to.y_ = to.y_ * BUCKET_GRID_SIZE - BUCKET_GRID_SIZE * 0.5;

	return mainGrid.getContentInfo(from, to, checks, activePlayer);
}
