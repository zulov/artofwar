#pragma once
#include <array>
#include <optional>
#include "ComplexBucketData.h"
#include "Grid.h"
#include "objects/resource/ResourceEntity.h"
#include "../path/PathFinder.h"


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
	void updateNeight(int idx) const;

	void prepareGridToFind() const;
	bool validateAdd(const Urho3D::IntVector2& size, Urho3D::IntVector2 bucketCords, bool isBuilding) const;
	void addStatic(Static* object, bool bulkAdd);
	void removeStatic(Static* object) const;
	std::optional<Urho3D::Vector2> getDirectionFrom(int index, const Urho3D::Vector3& position) const;

	Urho3D::Vector2 getValidPosition(const Urho3D::IntVector2& size, const Urho3D::IntVector2& cords) const;

	void updateNeighbors(ComplexBucketData& data, int dataIndex) const;

	const std::vector<int>* findPath(int startIdx, int endIdx, int limit);
	const std::vector<int>* findPath(int startIdx, const std::vector<int>& endIdxs, int limit, bool closeEnough);

	void drawComplex(Urho3D::Image* image, Urho3D::String prefix) const;

	Urho3D::Vector2 repulseObstacle(Unit* unit) const;
	void invalidatePathCache();

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
	void addResourceBonuses(Building* building, std::vector<int>& changedIndexes) const;

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
	void addResBonuses(std::vector<Building*>& resBuildings) const;
	void reAddBonuses(std::vector<Building*>& resBuildings, std::vector<ResourceEntity*>* resources) const;
	void refreshAllStatic(const std::span<int> allChanged);
	void refreshGradient(const std::vector<int>& notPassables) const;
	void refreshStatic(const std::span<int> changed);
	const std::vector<std::pair<unsigned char, short>>& getCloseTabIndexesWithValue(int center) const;
	const std::vector<short>& getCloseIndexes(int center) const;
private:
	void refreshAllGradient(std::vector<int>& toRefresh) const;
	void refreshGradientRemoveStatic(std::span<int> toRefresh) const;
	void createGradient(std::vector<int>& toRefresh, short level) const;

	float getBonuses(char player, const ResourceEntity* resource) const;

	ComplexBucketData* complexData;
	PathFinder pathFinder;
	std::array<Urho3D::Vector2, 4> posInBucket;

	int counter = 0;
};

