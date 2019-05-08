#pragma once
#include "ComplexBucketData.h"
#include "Grid.h"
#include "objects/resource/ResourceEntity.h"
#include "objects/unit/state/UnitState.h"
#include "path/PathFinder.h"


class Unit;

namespace Urho3D {
	class Image;
}

class content_info;

class MainGrid : public Grid
{
public:
	MainGrid(short _resolution, float _size);
	~MainGrid();

	void prepareGridToFind();
	bool validateAdd(Static* object);
	bool validateAdd(const Urho3D::IntVector2& size, Urho3D::Vector2& pos);
	void addStatic(Static* object);
	void removeStatic(Static* object);
	Urho3D::Vector2* getDirectionFrom(Urho3D::Vector3* position);
	Urho3D::Vector2 getValidPosition(const Urho3D::IntVector2& size, const Urho3D::Vector2& pos);
	Urho3D::IntVector2 getBucketCords(const Urho3D::IntVector2& size, const Urho3D::Vector2& pos) const;

	void updateNeighbors(int current) const;
	float cost(int current, int next) const;

	int getCloseIndex(int center, int i) const;

	std::vector<int>* findPath(int startIdx, const Urho3D::Vector2& aim);

	void drawMap(Urho3D::Image* image);
	content_info* getContentInfo(const Urho3D::Vector2& from, const Urho3D::Vector2& to, bool checks[],
	                             int activePlayer);
	Urho3D::Vector2 repulseObstacle(Unit* unit);
	void invalidateCache();
	void updateSurround(Static* object);

	Urho3D::Vector2 getPositionInBucket(int index, char max, char i);

	Urho3D::IntVector2 getCords(int index) const { return Urho3D::IntVector2(index / resolution, index % resolution); }

	Urho3D::Vector2& getCenterAt(const Urho3D::IntVector2& cords) const {
		return complexData[getIndex(cords.x_, cords.y_)].getCenter();
	}

	Urho3D::Vector2& getCenter(short x, short z) { return getCenter(getIndex(x, z)); }
	Urho3D::Vector2& getCenter(int index) const { return complexData[index].getCenter(); }

	bool inSide(int x, int z) const { return !(x < 0 || x >= resolution || z < 0 || z >= resolution); }
	bool inSide(int index) const { return !(index < 0 || index > sqResolution); }
	CellState getType(int index) const { return complexData[index].getType(); }
	char getCurrentSize(int index) const { return complexData[index].getSize(); }
	bool cellInStates(int index, std::vector<CellState>& cellStates) const;
	void updateCell(int index, char val, CellState cellState) const;
	bool belowCellLimit(int index) const;
	char getNumberInState(int index, UnitState state) const;
	char getOrdinalInState(Unit* unit, UnitState state) const;
	int getRevertCloseIndex(int center, int gridIndex);
	void addDeploy(Building* building);
	void removeDeploy(Building* building);

private:
	void updateInfo(int index, content_info* ci, bool* checks, int activePlayer);

	PathFinder *pathConstructor;

	content_info* ci;

	std::vector<Urho3D::Vector2> posInBucket2;
	std::vector<Urho3D::Vector2> posInBucket3;
	std::vector<Urho3D::Vector2> posInBucket4;

	ComplexBucketData* complexData;

	std::vector<short> closeIndex;
};
