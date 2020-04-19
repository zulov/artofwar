#pragma once
#include "ComplexBucketData.h"
#include "Grid.h"
#include "objects/resource/ResourceEntity.h"
#include "objects/unit/state/UnitState.h"
#include "path/PathFinder.h"

#define GRID_DEBUG_SIZE 3
class Unit;

namespace Urho3D {
	class Image;
}

struct content_info;


enum class GridDebugType : char {
	NONE,
	GRID,
	CELLS_TYPE
};

class MainGrid : public Grid {
public:
	MainGrid(short _resolution, float _size);
	~MainGrid();

	void prepareGridToFind() const;
	bool validateAdd(Static* object) const;
	bool validateAdd(const Urho3D::IntVector2& size, Urho3D::Vector2& pos) const;
	void addStatic(Static* object);
	void removeStatic(Static* object) const;
	Urho3D::Vector2* getDirectionFrom(Urho3D::Vector3& position);
	std::pair<Urho3D::IntVector2, Urho3D::Vector2> getValidPosition(const Urho3D::IntVector2& size, const Urho3D::Vector2& pos) const;

	void updateNeighbors(int current) const;
	float cost(int current, int next) const;

	std::vector<int>* findPath(int startIdx, const Urho3D::Vector2& aim) const;
	std::vector<int>* findPath(const Urho3D::Vector3& from, const Urho3D::Vector2& aim) const;

	void drawMap(Urho3D::Image* image) const;

	Urho3D::Vector2 repulseObstacle(Unit* unit);
	void invalidateCache() const;
	void updateSurround(Static* object);

	Urho3D::Vector2 getPositionInBucket(int index, char max, char i);

	Urho3D::IntVector2 getCords(int index) const { return Urho3D::IntVector2(index / resolution, index % resolution); }

	Urho3D::Vector2& getCenterAt(const Urho3D::IntVector2& cords) const {
		return complexData[calculator.getIndex(cords.x_, cords.y_)].getCenter();
	}

	Urho3D::Vector2& getCenter(short x, short z) const { return getCenter(calculator.getIndex(x, z)); }
	Urho3D::Vector2& getCenter(int index) const { return complexData[index].getCenter(); }

	CellState getType(int index) const { return complexData[index].getType(); }
	char getCurrentSize(int index) const { return complexData[index].getSize(); }
	bool cellInStates(int index, std::vector<CellState>& cellStates) const;
	void updateCell(int index, char val, CellState cellState) const;
	bool belowCellLimit(int index) const;
	char getNumberInState(int index, UnitState state) const;
	char getOrdinalInState(Unit* unit, UnitState state) const;
	int getRevertCloseIndex(int center, int gridIndex);
	void addDeploy(Building* building) const;
	void removeDeploy(Building* building) const;

	bool isInLocalArea(int getMainCell, Urho3D::Vector2& pos);
	bool isEmpty(int inx) const;
	int closestEmpty(int posIndex);
	int indexFromPosition(const Urho3D::Vector2& pos) const { return calculator.indexFromPosition(pos); }
	int getIndex(short i, short z) const { return calculator.getIndex(i, z); }
	int getIndex(Urho3D::Vector2& pos) const { return calculator.indexFromPosition(pos); }
	CellState getCellAt(float x, float z) const;
	int getAdditionalInfoAt(float x, float z) const;
	void drawDebug(GridDebugType type) const;
	bool validAndFree(short id, int index, std::vector<short>::value_type close) const;
	Urho3D::Vector2 getNewBuildingPos(const Urho3D::Vector2& center, char player, short id);
	float getFieldSize();
private:

	PathFinder* pathConstructor;

	std::vector<Urho3D::Vector2> posInBucket2;
	std::vector<Urho3D::Vector2> posInBucket3;
	std::vector<Urho3D::Vector2> posInBucket4;

	ComplexBucketData* complexData;
};
