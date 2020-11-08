#pragma once
#include <array>
#include <optional>
#include "ComplexBucketData.h"
#include "Grid.h"
#include "objects/unit/state/UnitState.h"
#include "path/PathFinder.h"
#include "simulation/env/GridCalculator.h"


namespace Urho3D
{
	class Image;
}

class Unit;
struct content_info;

enum class GridDebugType : char {
	NONE,
	GRID,
	CELLS_TYPE
};

class MainGrid : public Grid {
public:
	MainGrid(short resolution, float size, float maxQueryRadius);
	MainGrid(const MainGrid& rhs) = delete;
	~MainGrid();

	void prepareGridToFind() const;
	bool validateAdd(Static* object) const;
	bool validateAdd(const Urho3D::IntVector2& size, Urho3D::Vector2& pos) const;
	void addStatic(Static* object) const;
	void removeStatic(Static* object) const;
	std::optional<Urho3D::Vector2> getDirectionFrom(Urho3D::Vector3& position) const;
	std::pair<Urho3D::IntVector2, Urho3D::Vector2> getValidPosition(const Urho3D::IntVector2& size,
	                                                                const Urho3D::Vector2& pos) const;

	void updateNeighbors(int current) const;
	float cost(int current, int next) const;

	std::vector<int>* findPath(int startIdx, const Urho3D::Vector2& aim) const;
	std::vector<int>* findPath(const Urho3D::Vector3& from, const Urho3D::Vector2& aim) const;

	void drawComplex(Urho3D::Image* image, Urho3D::String prefix) const;

	Urho3D::Vector2 repulseObstacle(Unit* unit) const;
	void invalidateCache() const;
	void updateSurround(Static* object) const;

	Urho3D::Vector2 getPositionInBucket(int index, char max, char i);

	Urho3D::IntVector2 getCords(int index) const { return Urho3D::IntVector2(index / resolution, index % resolution); }

	Urho3D::Vector2 getCenterAt(const Urho3D::IntVector2& cords) const {
		return getCenter(calculator->getIndex(cords.x_, cords.y_));
	}

	Urho3D::Vector2 getCenter(short x, short z) const { return getCenter(calculator->getIndex(x, z)); }
	Urho3D::Vector2 getCenter(int index) const { return calculator->getCenter(index); }

	bool cellInState(int index, CellState state) const;
	void updateCell(int index, char val, CellState cellState) const;
	char getNumberInState(int index, UnitState state) const;
	char getOrdinalInState(Unit* unit, UnitState state) const;
	int getRevertCloseIndex(int center, int gridIndex) const;
	void addDeploy(Building* building) const;
	void removeDeploy(Building* building) const;

	bool isInLocalArea(int cell, Urho3D::Vector2& pos) const;
	bool isPassable(int inx) const;
	bool isBuildable(int inx) const;
	int closestPassableCell(int posIndex) const;
	int getIndex(short i, short z) const { return calculator->getIndex(i, z); }
	int getIndex(Urho3D::Vector2& pos) const { return calculator->indexFromPosition(pos); }
	CellState getCellAt(float x, float z) const;
	int getAdditionalInfoAt(float x, float z) const;
	void drawDebug(GridDebugType type) const;
	float getFieldSize() const;
	void drawAll();
	bool cellIsCollectable(int index) const;
private:

	PathFinder* pathConstructor;

	std::array<Urho3D::Vector2, 2> posInBucket2;
	std::array<Urho3D::Vector2, 3> posInBucket3;
	std::array<Urho3D::Vector2, 4> posInBucket4;

	ComplexBucketData* complexData;
	int counter = 0;
};
