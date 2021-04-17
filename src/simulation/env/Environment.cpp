#include "Environment.h"
#include <Urho3D/Graphics/Terrain.h>
#include "EnvConsts.h"
#include "database/db_strcut.h"
#include "math/MathUtils.h"
#include "objects/building/Building.h"
#include "objects/resource/ResourceEntity.h"
#include "objects/unit/Unit.h"
#include "utils/consts.h"


Environment::Environment(Urho3D::Terrain* terrian):
	mainGrid(BUCKET_GRID_RESOLUTION, BUCKET_GRID_SIZE, 24),
	resourceGrid(BUCKET_GRID_RESOLUTION_RESOURCE, BUCKET_GRID_SIZE, 256),
	buildingGrid(BUCKET_GRID_RESOLUTION_BUILD, BUCKET_GRID_SIZE, 256),
	teamUnitGrid{
		{BUCKET_GRID_RESOLUTION_ENEMY, BUCKET_GRID_SIZE, 256},
		{BUCKET_GRID_RESOLUTION_ENEMY, BUCKET_GRID_SIZE, 256}
	}, influenceManager(MAX_PLAYERS), terrain(terrian) {
	neights = new std::vector<Physical*>();
	neights2 = new std::vector<Physical*>();
}


Environment::~Environment() {
	delete neights;
	delete neights2;
}

std::vector<Physical*>* Environment::getNeighboursFromTeamEq(Physical* physical, const float radius, const int team) {
	return getNeighbours(physical, teamUnitGrid[team], radius);
}

std::vector<Physical*>* Environment::getNeighboursFromTeamNotEq(Physical* physical,
                                                                const float radius, const int team) {
	neights2->clear();
	for (int i = 0; i < MAX_PLAYERS; ++i) {
		if (team != i) {
			auto neightLocal = getNeighbours(physical, teamUnitGrid[i], radius);
			neights2->insert(neights2->end(), neightLocal->begin(), neightLocal->end());
		}
	}
	return neights2;
}

std::vector<Physical*>* Environment::getNeighbours(Physical* physical, Grid& bucketGrid, float radius) const {
	neights->clear();

	const auto& center = physical->getPosition();
	BucketIterator& bucketIterator = bucketGrid.getArrayNeight(physical->getPosition(), radius);
	const float sqRadius = radius * radius;

	while (Physical* neight = bucketIterator.next()) {
		addIfInRange(physical, center, sqRadius, neight);
	}

	return neights;
}

void Environment::addIfInRange(Physical* physical, const Urho3D::Vector3& center, const float sqRadius,
                               Physical* neight) const {
	if (physical != neight
		&& sqDistAs2D(center, neight->getPosition()) < sqRadius) {
		neights->push_back(neight);
	}
}

std::vector<Physical*>* Environment::getNeighboursWithCache(Unit* unit, float radius) {
	const auto currentIdx = unit->getMainBucketIndex();
	assert(currentIdx>=0);
	if (mainGrid.onlyOneInside(currentIdx)) {
		return getNeighbours(unit, mainGrid, radius);
	}
	const auto simpleNeght = mainGrid.getAllFromCache(currentIdx, radius);

	const float sqRadius = radius * radius;

	neights->clear();
	auto pred = [sqRadius,unit](Physical* neight) {
		return (unit != neight && sqDistAs2D(unit->getPosition(), neight->getPosition()) < sqRadius);
	};
	std::copy_if(simpleNeght->begin(), simpleNeght->end(), std::back_inserter(*neights), pred);

	return neights;
}

std::vector<Physical*>* Environment::getNeighbours(Urho3D::Vector3& center, Grid& bucketGrid, float radius,
                                                   int id) const {
	neights->clear();

	BucketIterator& bucketIterator = bucketGrid.getArrayNeight(center, radius);
	const float sqRadius = radius * radius;

	while (Physical* neight = bucketIterator.next()) {
		if (id == neight->getId() && sqDistAs2D(center, neight->getPosition()) < sqRadius) {
			neights->push_back(neight);
		}
	}

	return neights;
}

const std::vector<Physical*>* Environment::getNeighboursSimilarAs(Physical* clicked) const {
	if (!clicked) {
		return &Consts::EMPTY_PHYSICAL;
	}

	return grids[cast(clicked->getType())]->getArrayNeightSimilarAs(clicked, 20.f);
}

std::vector<Physical*>* Environment::getResources(Physical* physical, float radius) {
	return getNeighbours(physical, resourceGrid, radius);
}

std::vector<Physical*>* Environment::getResources(Urho3D::Vector3& center, float radius, int id) {
	return getNeighbours(center, resourceGrid, radius, id);
}

void Environment::updateInfluenceUnits1(std::vector<Unit*>* units) const {
	influenceManager.update(units);
}

void Environment::updateInfluenceUnits2(std::vector<Unit*>* units) const {
	influenceManager.updateQuadUnits(units);
}

void Environment::updateInfluenceResources(std::vector<ResourceEntity*>* resources) const {
	influenceManager.update(resources);
}

void Environment::updateInfluenceOther(std::vector<Building*>* buildings) const {
	influenceManager.update(buildings);
	influenceManager.updateQuadBuildings(buildings);
	influenceManager.updateWithHistory();
}

void Environment::updateQuadOther() const {
	influenceManager.updateQuadOther();
}

void Environment::update(Unit* unit) const {
	mainGrid.update(unit);
	teamUnitGrid[unit->getTeam()].update(unit, unit->getTeam());
}

void Environment::addNew(const std::vector<Unit*>& units) {
	for (auto unit : units) {
		mainGrid.updateNew(unit);
		teamUnitGrid[unit->getTeam()].updateNew(unit, unit->getTeam());
	}

	invalidateCaches();
}

void Environment::invalidateCaches() {
	mainGrid.invalidateCache();
	for (auto& unitGrid : teamUnitGrid) {
		unitGrid.invalidateCache();
	}
}

void Environment::updateAll(std::vector<Building*>* const buildings) const {
	influenceManager.update(buildings);
}

void Environment::addNew(Building* building) const {
	mainGrid.addStatic(building);
	buildingGrid.updateNew(building);
	for (auto cell : building->getSurroundCells()) {
		if (mainGrid.isBuildable(cell)) {
			building->setDeploy(cell);
			break;
		}
	}
	assert(building->getDeploy().has_value());

	mainGrid.addDeploy(building);
}

void Environment::addNew(ResourceEntity* resource) const {
	mainGrid.addStatic(resource);
	resourceGrid.updateNew(resource);
}

Urho3D::Vector2 Environment::repulseObstacle(Unit* unit) const {
	return mainGrid.repulseObstacle(unit);
}

std::optional<Urho3D::Vector2> Environment::validatePosition(Urho3D::Vector3& position) const {
	return mainGrid.getDirectionFrom(position);
}

const std::vector<Physical*>* Environment::getNeighbours(std::pair<Urho3D::Vector3*, Urho3D::Vector3*>& pair,
                                                         char player) {
	for (auto grid : grids) {
		const auto result = grid->getArrayNeight(pair, player);
		if (!result->empty()) {
			return result;
		}
	}
	return &Consts::EMPTY_PHYSICAL;
}

float Environment::getGroundHeightAt(float x, float z) const {
	return terrain->GetHeight(Urho3D::Vector3(x, 0, z));
}

float Environment::getGroundHeightAt(Urho3D::Vector3& pos) const {
	return terrain->GetHeight(pos);
}

Urho3D::Vector3 Environment::getPosWithHeightAt(float x, float z) const {
	return Urho3D::Vector3(x, getGroundHeightAt(x, z), z);
}

Urho3D::Vector3 Environment::getPosWithHeightAt(int index) const {
	const auto center = mainGrid.getCenter(index);
	return getPosWithHeightAt(center.x_, center.y_);
}

float Environment::getGroundHeightPercent(float y, float x, float div) const {
	const float scale = terrain->GetSpacing().y_;
	const auto a = Urho3D::Vector3(x * BUCKET_GRID_SIZE - BUCKET_GRID_SIZE * 0.5f, 0.f,
	                               y * BUCKET_GRID_SIZE - BUCKET_GRID_SIZE * 0.5f);

	return terrain->GetHeight(a) / scale / div;
}

Urho3D::Vector3 Environment::getValidPosForCamera(float percentX, float percentY, const Urho3D::Vector3& pos,
                                                  float min) const {
	auto a = Urho3D::Vector3(percentX * BUCKET_GRID_SIZE - BUCKET_GRID_SIZE * 0.5f, pos.y_,
	                         percentY * BUCKET_GRID_SIZE - BUCKET_GRID_SIZE * 0.5f);
	const float h = terrain->GetHeight(a);
	if (h + min > pos.y_) {
		a.y_ = h + min;
	}
	return a;
}

bool Environment::validateStatic(const Urho3D::IntVector2& size, Urho3D::Vector2& pos) const {
	return mainGrid.validateAdd(size, pos);
}

bool Environment::validateStatic(const Urho3D::IntVector2& size, const Urho3D::IntVector2 bucketCords) const {
	return mainGrid.validateAdd(size, bucketCords);
}

Urho3D::Vector2 Environment::getCenter(int index) const {
	return mainGrid.getCenter(index);
}

Urho3D::Vector2 Environment::getCenter(short x, short z) const {
	return mainGrid.getCenter(x, z);
}

Urho3D::Vector2 Environment::getPositionInBucket(Unit* unit) {
	return mainGrid.getPositionInBucket(unit);
}

void Environment::invalidatePathCache() const {
	mainGrid.invalidatePathCache();
}

bool Environment::cellInState(int index, CellState state) const {
	return mainGrid.cellInState(index, state);
}

void Environment::updateCell(int index, char val, CellState cellState) const {
	mainGrid.updateCell(index, val, cellState);
}

void Environment::removeFromGrids(const std::vector<Unit*>& units) const {
	for (auto unit : units) {
		mainGrid.remove(unit);
		teamUnitGrid[unit->getTeam()].remove(unit, unit->getTeam());
	}
}

void Environment::removeFromGrids(const std::vector<Building*>& buildingsToDispose,
                                  const std::vector<ResourceEntity*>& resourceToDispose) const {
	for (auto building : buildingsToDispose) {
		mainGrid.removeStatic(building);
		mainGrid.removeDeploy(building);
		buildingGrid.remove(building);
	}
	for (auto resource : resourceToDispose) {
		mainGrid.removeStatic(resource);
		resourceGrid.remove(resource);
	}
}

unsigned char Environment::getRevertCloseIndex(int center, int gridIndex) const {
	return mainGrid.getRevertCloseIndex(center, gridIndex);
}

void Environment::drawDebug(EnvironmentDebugMode environmentDebugMode, char index) {
	switch (environmentDebugMode) {
	case EnvironmentDebugMode::NONE:
		influenceManager.draw(InfluenceDataType::NONE, index);
		break;
	case EnvironmentDebugMode::MAIN_GRID:
		mainGrid.drawDebug(GridDebugType(index % magic_enum::enum_count<GridDebugType>()));
		influenceManager.draw(InfluenceDataType::NONE, index);
		break;
	case EnvironmentDebugMode::INF_UNITS_NUMBER_PER_PLAYER:
		influenceManager.draw(InfluenceDataType::UNITS_NUMBER_PER_PLAYER, index);
		break;
	case EnvironmentDebugMode::INF_UNITS_INFLUENCE_PER_PLAYER:
		influenceManager.draw(InfluenceDataType::UNITS_INFLUENCE_PER_PLAYER, index);
		break;
	case EnvironmentDebugMode::INF_BUILDING_INFLUENCE_PER_PLAYER:
		influenceManager.draw(InfluenceDataType::BUILDING_INFLUENCE_PER_PLAYER, index);
		break;
	case EnvironmentDebugMode::INF_RESOURCE:
		influenceManager.draw(InfluenceDataType::RESOURCE_INFLUENCE, index);
		break;
	default: ;
	}
}

const std::vector<short>& Environment::getCloseIndexs(int center) const {
	return mainGrid.getCloseIndexes(center);
}

const std::vector<unsigned char>& Environment::getCloseTabIndexes(int center) const {
	return mainGrid.getCloseTabIndexes(center);
}

std::array<float, 5>& Environment::getInfluenceDataAt(char player, const Urho3D::Vector2& pos) {
	return influenceManager.getInfluenceDataAt(player, pos);
}

std::optional<Urho3D::Vector2> Environment::getPosToCreate(db_building* building, char player,
                                                           const std::span<float> result) {
	std::vector<int> indexes = influenceManager.getAreas(result, player);
	//TODO performance nie obliczać wszystkich centrów tylko te co trzeba
	float ratio = influenceManager.getFieldSize() / mainGrid.getFieldSize();
	for (auto centerIndex : indexes) {
		for (auto index : mainGrid.getCloseCenters(centerIndex, ratio)) {
			auto gridCenter = mainGrid.getCenter(index);
			if (validateStatic(building->size, gridCenter)) {
				return gridCenter;
			}
		}
	}
	return {};
}

std::vector<Urho3D::Vector2> Environment::getAreas(char player, const std::span<float> result, int min) {
	return influenceManager.getAreasIterative(result, player, 0.1, min);
}

void Environment::addCollect(Unit* unit, float value) {
	influenceManager.addCollect(unit, value);
}

void Environment::addAttack(Unit* unit, float value) {
	influenceManager.addAttack(unit, value);
}

void Environment::drawInfluence() {
	influenceManager.drawAll();
	mainGrid.drawAll();
}

bool Environment::cellIsPassable(int index) const {
	return mainGrid.isPassable(index);
}

bool Environment::cellIsCollectable(int index) const {
	return mainGrid.cellIsCollectable(index);
}

std::optional<Urho3D::Vector2> Environment::getCenterOf(CenterType id, char player) {
	return influenceManager.getCenterOf(id, player);
}

bool Environment::anyCloseEnough(std::vector<int> const& indexes, int center, float distThreshold) {
	return mainGrid.anyCloseEnough(indexes, center, distThreshold);
}

bool Environment::isInLocalArea(int getMainCell, int aimIndex) const {
	return mainGrid.isInLocalArea(getMainCell, aimIndex);
}

bool Environment::isInLocal2Area(int getMainCell, int aimIndex) const {
	return mainGrid.isInLocal2Area(getMainCell, aimIndex);
}

int Environment::closestPassableCell(int posIndex) const {
	return mainGrid.closestPassableCell(posIndex);
}

Urho3D::Vector2 Environment::getValidPosition(const Urho3D::IntVector2& size, const Urho3D::Vector2& pos) const {
	return mainGrid.getValidPosition(size, pos);
}

Urho3D::Vector2 Environment::getValidPosition(const Urho3D::IntVector2& size,
                                              const Urho3D::IntVector2& bucketCords) const {
	return mainGrid.getValidPosition(size, mainGrid.getCenterAt(bucketCords));
}

std::vector<int>* Environment::findPath(int startIdx, const Urho3D::Vector2& aim, int limit) const {
	return mainGrid.findPath(startIdx, aim, limit);
}

std::vector<int>* Environment::findPath(int startIdx, const std::vector<int>& endIdxs, int limit) const {
	return mainGrid.findPath(startIdx, endIdxs, limit);
}

std::vector<int>* Environment::findPath(int startIdx, int endIdx, int limit) const {
	return mainGrid.findPath(startIdx, endIdx, limit);
}

void Environment::prepareGridToFind() const {
	mainGrid.prepareGridToFind();
}

content_info* Environment::getContentInfo(Urho3D::Vector2 centerPercent, bool checks[], int activePlayer) {
	const float x = getPositionFromPercent(centerPercent.x_);
	const float z = getPositionFromPercent(centerPercent.y_);
	const CellState state = mainGrid.getCellAt(x, z);
	const int addInfo = mainGrid.getAdditionalInfoAt(x, z);
	return influenceManager.getContentInfo({x, z}, state, addInfo, checks, activePlayer);
}

float Environment::getPositionFromPercent(float value) const {
	return BUCKET_GRID_SIZE * (value - 0.5);
}
