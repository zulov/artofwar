#pragma once
#include <array>
#include <optional>
#include "ComplexBucketData.h"
#include "Grid.h"
#include "path/PathFinder.h"


namespace Urho3D {
	class Image;
}

class Unit;
struct content_info;

enum class GridDebugType : char {
	NONE,
	CELLS_TYPE
};

class MainGrid : public Grid {
public:
	MainGrid(short resolution, float size, float maxQueryRadius);
	MainGrid(const MainGrid& rhs) = delete;
	~MainGrid() override;

	void prepareGridToFind() const;
	bool validateAdd(const Urho3D::IntVector2& size, Urho3D::Vector2& pos) const;
	bool validateAdd(const Urho3D::IntVector2& size, Urho3D::IntVector2 bucketCords) const;
	void addStatic(Static* object);
	void removeStatic(Static* object) const;
	std::optional<Urho3D::Vector2> getDirectionFrom(int index, const Urho3D::Vector3& position) const;
	Urho3D::Vector2 getValidPosition(const Urho3D::IntVector2& size, const Urho3D::Vector2& pos) const;
	Urho3D::Vector2 getValidPosition(const Urho3D::IntVector2& size, const Urho3D::IntVector2& cords) const;

	void updateNeighbors(ComplexBucketData& data, int current) const;
	float cost(const Urho3D::IntVector2& centerParams, int next) const;

	std::vector<int>* findPath(int startIdx, int endIdx, int limit);
	std::vector<int>* findPath(int startIdx, const std::vector<int>& endIdxs, int limit);

	void drawComplex(Urho3D::Image* image, Urho3D::String prefix) const;

	Urho3D::Vector2 repulseObstacle(Unit* unit) const;
	void invalidatePathCache();

	Urho3D::Vector2 getPositionInBucket(Unit* unit);

	bool cellInState(int index, CellState state) const;
	void updateCell(int index, char val, CellState cellState) const;

	unsigned char getRevertCloseIndex(int center, int gridIndex) const;
	void addDeploy(Building* building) const;
	void removeDeploy(Building* building) const;

	bool isInLocalArea(int center, int indexOfAim) const;
	bool isInLocal1and2Area(int center, int indexOfAim) const;

	bool isPassable(int inx) const;
	bool isBuildable(int inx) const;
	int closestPassableCell(int posIndex) const;

	CellState getCellAt(float x, float z) const;
	int getAdditionalInfoAt(float x, float z) const;
	void drawDebug(GridDebugType type) const;
	Urho3D::Vector3 getVertex(Urho3D::Vector2 center, Urho3D::Vector2 vertex) const;
	float getFieldSize() const;
	void drawAll();
	bool cellIsCollectable(int index) const;
	bool cellIsAttackable(int index) const;
	bool anyCloseEnough(std::vector<int> const& indexes, int center, float distThreshold) const;
	std::vector<int> getIndexesInRange(const Urho3D::Vector3& center, float range) const;

private:
	ComplexBucketData* complexData;
	PathFinder pathFinder;

	std::array<Urho3D::Vector2, 4> posInBucket4;
	int counter = 0;
};
