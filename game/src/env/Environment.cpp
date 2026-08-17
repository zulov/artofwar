#include "Environment.h"

#include <cassert>
#include <Urho3D/Graphics/Material.h>
#include <Urho3D/Graphics/Terrain.h>
#include "EnvConsts.h"
#include "GridCalculator.h"
#include "GridCalculatorProvider.h"
#include "control/MouseButton.h"
#include "database/db_struct.h"
#include "math/MathUtils.h"
#include "math/VectorUtils.h"
#include "objects/building/Building.h"
#include "objects/resource/ResourceEntity.h"
#include "objects/unit/Unit.h"
#include "utils/consts.h"
#include "utils/OtherUtils.h"


Environment::Environment(Urho3D::Terrain* terrain, unsigned short mainMapResolution):
	mapSize(mainMapResolution * BUCKET_GRID_FIELD_SIZE),
	mainGrid(mainMapResolution, mapSize, 24),
	buildingGrid(mapSize / BUCKET_GRID_FIELD_SIZE_BUILD, mapSize, 256.f),
	resourceStaticGrid(mapSize / BUCKET_GRID_FIELD_SIZE_RESOURCE, mapSize, RESOURCE_GRID_QUERY_RADIUS_LEVELS),
	sparseUnitGrid((short)(mapSize / BUCKET_GRID_FIELD_SIZE_ENEMY), mapSize, 256.f),
	terrain(terrain), influenceManager(MAX_PLAYERS, mapSize, terrain),
	calculator(GridCalculatorProvider::get(mainMapResolution, mapSize)) {
	auto a = {160, 192, 256, 320, 384, 512};
	assert(std::ranges::any_of(a, [mainMapResolution](int i) {return mainMapResolution == i; }));
}


Environment::~Environment() = default;


const std::vector<Physical*>& Environment::getNeighboursFromSparseSamePlayer(const Physical* source,
                                                                              const Urho3D::Vector2& center,
                                                                              float radius, char playerId) {
	return getNeighbours(source, center, sparseUnitGrid, radius,
	                     [playerId](const Physical* neighbor) { return neighbor->getPlayer() == playerId && neighbor->isAlive(); });
}

const std::vector<Physical*>& Environment::getNeighboursFromTeamNotEq(const Physical* source,
                                                                       const Urho3D::Vector2& center,
                                                                       char playerId, float radius) {
	return getNeighbours(source, center, sparseUnitGrid, radius,
	                     [playerId](const Physical* neighbor) { return neighbor->getPlayer() != playerId && neighbor->isAlive(); });
}

bool Environment::isVisible(char player, const Urho3D::Vector2& pos) const {
	return influenceManager.isVisible(player, pos);
}

float Environment::getVisibilityScore(char player) const {
	return influenceManager.getVisibilityScore(player);
}

std::vector<int> Environment::getIndexesInRange(int index, float range) const {
	return mainGrid.getIndexesInRange(index, range);
}

std::vector<int> Environment::getIndexesInRange(const Urho3D::Vector2& center, float range) const {
	return mainGrid.getIndexesInRange(center, range);
}

void Environment::setTerrainShaderParam(const Urho3D::String& name, const Urho3D::Variant& value) const {
	if (terrain && !SIM_GLOBALS.HEADLESS) {
		terrain->GetMaterial()->SetShaderParameter(name, value);
	}
}

void Environment::flipTerrainShaderParam(const Urho3D::String& name) const {
	if (terrain && !SIM_GLOBALS.HEADLESS) {
		const auto mat = terrain->GetMaterial();

		mat->SetShaderParameter(name, !mat->GetShaderParameter(name).GetBool());
	}
}

void Environment::nextVisibilityType() const {
	influenceManager.nextVisibilityType();
}

void Environment::reAddBonuses(std::span<Building* const> resourceBuildings,
                               std::span<ResourceEntity* const> resources) const {
	mainGrid.reAddBonuses(resourceBuildings, resources);
}

void Environment::refreshAllStatic(std::span<ResourceEntity* const> resources, std::span<Building* const> buildings) {
	mainGrid.refreshAllStatic(resources, buildings);
	mainGrid.invalidatePathCache();
}

short Environment::getOccupationLevel(int index) const {
	return mainGrid.getGradient(index);
}

const std::vector<Physical*>& Environment::getNeighbours(const Physical* source, const Urho3D::Vector2& center,
                                                         Grid& grid, float radius,
                                                         const std::function<bool(const Physical*)>& condition) const {
	neighbors.clear();
	BucketIterator& bucketIterator = grid.getArrayNeight(center, radius);
	const float squaredRadius = radius * radius;

	while (Physical* neighbor = bucketIterator.next()) {
		addIfInRange(source, center, neighbor, squaredRadius, condition);
	}

	return neighbors;
}

const std::vector<Physical*>& Environment::getResources(const Urho3D::Vector2& center, float radius) {
	neighbors.clear();

	BucketIterator& bucketIterator = resourceStaticGrid.getArrayNeight(center, radius);
	const float sqRadius = radius * radius;

	while (Physical* neighbor = bucketIterator.next()) {
		if (center.SqDistXZ(neighbor->getPosition()) <= sqRadius) {
			neighbors.push_back(neighbor);
		}
	}

	return neighbors;
}

std::vector<int> Environment::getUniqueResourceIndexesInRange(const std::unordered_set<int>& centerIndexes,
                                                               float radius) const {
	std::vector<int> indexes;
	indexes.reserve(100);
	for (const auto centerIndex : centerIndexes) {
		resourceStaticGrid.appendIndexesInRange(getCenter(centerIndex), radius, indexes);
	}
	sortAndRemoveDuplicates(indexes);
	return indexes;
}

const std::vector<Physical*>& Environment::getResourcesAt(int cellIndex) const {
	return resourceStaticGrid.getContentAt(cellIndex);
}

void Environment::addIfInRange(const Physical* source, const Urho3D::Vector2& center, Physical* neighbor,
                               float sqRadius,
                               const std::function<bool(const Physical*)>& condition) const {
	if (source != neighbor && (!condition || condition(neighbor))
		&& center.SqDistXZ(neighbor->getPosition()) < sqRadius) {
		neighbors.push_back(neighbor);
	}
}

const std::vector<Physical*>& Environment::getNeighboursWithCache(const Physical* source,
                                                                  const Urho3D::Vector2& center,
                                                                  int gridIndex, float radius) {
	assert(gridIndex >= 0);
	if (mainGrid.onlyOneInside(gridIndex)) {
		return getNeighbours(source, center, mainGrid, radius);
	}//TODO czy ten cache cos wogole daje?
	const auto cachedNeighbors = mainGrid.getAllFromCache(gridIndex, radius);

	const float squaredRadius = radius * radius;
	neighbors.clear();
	auto isInRange = [source, squaredRadius, &center](const Physical* neighbor) {
		return (source != neighbor && center.SqDistXZ(neighbor->getPosition()) < squaredRadius);
	};
	std::ranges::copy_if(*cachedNeighbors, std::back_inserter(neighbors), isInRange);

	return neighbors;
}

const std::vector<Physical*>& Environment::getNeighboursSimilarAs(const ObjectType objectType,
                                                                  const Urho3D::Vector2& center,
                                                                  unsigned short databaseId, char playerId) const {
	return *grids[castC(objectType)]->getArrayNeightSimilarAs(center, databaseId, playerId, 20.f);
}

const std::vector<Physical*>&
Environment::getResources(const Urho3D::Vector2& center, int resourceId, int resourceLevel) {
	const float innerRadius = resourceLevel > 0
		? resourceStaticGrid.getRadiusForLevel(resourceLevel - 1) : -1.f;
	const float radius = resourceStaticGrid.getRadiusForLevel(resourceLevel);
	const float sqRadius = radius * radius;
	const float sqInnerRadius = innerRadius < 0.f ? innerRadius : innerRadius * innerRadius;
	neighbors.clear();
	for (auto neighbor : resourceStaticGrid.get(center, resourceLevel)) {
		if (resourceId == -1 || resourceId == static_cast<ResourceEntity*>(neighbor)->getResourceId()) {
			const float distance = center.SqDistXZ(neighbor->getPosition());
			if (distance <= sqRadius && distance > sqInnerRadius) {
				neighbors.push_back(neighbor);
			}
		}
	}
	return neighbors;
}

const std::vector<Physical*>&
Environment::getBuildingsFromTeamNotEq(const Physical* source, const Urho3D::Vector2& center, char teamId,
                                       int buildingId, float radius) {
	auto condition = [buildingId, teamId](const Physical* building) {
		return (buildingId < 0 || building->getDbId() == buildingId)
			&& (building->getTeam() != teamId || teamId < 0);
	};
	return getNeighbours(source, center, buildingGrid, radius, condition);
}

void Environment::updateInfluenceUnits(std::span<Unit* const> units) const {
	influenceManager.updateUnits(units);
}

void Environment::updateInfluenceBuildings(std::span<Building* const> buildings) const {
	influenceManager.updateBuildings(buildings);
	influenceManager.updateWithHistory();
}

void Environment::updateVisibility(std::span<Building* const> buildings, std::span<Unit* const> units,
                                   std::span<ResourceEntity* const> resources) const {
	influenceManager.updateVisibility(buildings, units, resources);
}

void Environment::update(Unit* unit) const {
	if (unit->isAlive()) {
		unit->setBucketInMainGrid(mainGrid.update(unit, unit->getMainGridIndex(), true));
		if (unit->indexChanged()) {
			unit->setSparseIndex(sparseUnitGrid.update(unit, unit->getSparseIndex(), false));
			unit->setIndexInInfluence(-1);
		}
	}
}

void Environment::addNew(std::span<Unit* const> units) {
	for (const auto unit : units) {
		assert(unit->getMainGridIndex() == -1);
		unit->setIndexChanged(true);
		unit->setBucketInMainGrid(mainGrid.updateNew(unit));
		unit->setSparseIndex(sparseUnitGrid.updateNew(unit));
	}

	invalidateCaches();
}

void Environment::invalidateCaches() {
	mainGrid.invalidateCache();
	sparseUnitGrid.invalidateCache();
}

void Environment::addNew(Building* building, bool bulkAdd) {
	mainGrid.addStatic(building, bulkAdd);
	mainGrid.addResBonuses(std::span<Building* const>(&building, 1));
	buildingGrid.updateNew(building);

	building->setIndexInInfluence(influenceManager.getIndex(building->getPosition()));

	for (const auto cell : building->getSurroundCells()) {
		if (mainGrid.isBuildable(cell)) {
			building->setDeploy(cell);
			break;
		}
	}
	assert(building->getDeploy().has_value());

	mainGrid.addDeploy(building);
	if (!bulkAdd) {
		mainGrid.invalidatePathCache();
	}
}

void Environment::addNew(ResourceEntity* resource, bool bulkAdd) {
	mainGrid.addStatic(resource, bulkAdd);
	resource->setIndexInInfluence(influenceManager.getIndex(resource->getPosition()));
	resourceStaticGrid.updateStatic(resource, bulkAdd);
	if (!bulkAdd) {
		mainGrid.invalidatePathCache();
	}
}

Urho3D::Vector2 Environment::repulseObstacle(Unit* unit) {
	return mainGrid.repulseObstacle(unit);
}

std::optional<Urho3D::Vector2> Environment::validatePosition(int index, const Urho3D::Vector2& position) const {
	return mainGrid.getDirectionFrom(index, position);
}


const std::vector<Physical*>& Environment::getNeighbours(MouseHeld& held, char playerId) {
	for (const auto grid : grids) {
		const auto result = grid->getArrayNeight(held, playerId);
		if (!result->empty()) {
			return *result;
		}
	}
	return Consts::EMPTY_PHYSICAL;
}

float Environment::getGroundHeightAt(float x, float z) const {
	if (terrain != nullptr) {
		return terrain->GetHeight(Urho3D::Vector3(x, 0.f, z));
	}
	return 0.f;
}

float Environment::getGroundHeightAt(const Urho3D::Vector2& pos) const {
	return getGroundHeightAt(pos.x_, pos.y_);
}

Urho3D::Vector3 Environment::getPosWithHeightAt(float x, float z) const {
	return {x, getGroundHeightAt(x, z), z};
}

Urho3D::Vector3 Environment::getPosWithHeightAt(int index) const {
	const auto center = calculator->getCenter(index);
	return getPosWithHeightAt(center.x_, center.y_);
}

float Environment::getGroundHeightPercentScaled(float x, float z, float div) const {
	if (terrain != nullptr) {
		return getGroundHeightAt(getPosFromPercent(x), getPosFromPercent(z)) / terrain->GetSpacing().y_ / div;
	}
	return 0.f;
}

bool Environment::validateStatic(const Urho3D::UCharVector2& size, const Urho3D::Vector2& position,
                                 bool isBuilding) const {
	return mainGrid.validateAdd(size,
	                            {calculator->getIndex(position.x_), calculator->getIndex(position.y_)}, isBuilding);
}

bool Environment::validateStatic(const Urho3D::UCharVector2& size, const Urho3D::UShortVector2& bucketCords,
                                 bool isBuilding) const {
	return mainGrid.validateAdd(size, bucketCords, isBuilding);
}

bool Environment::validateStatic(const Urho3D::UCharVector2& size, int index, bool isBuilding) const {
	return mainGrid.validateAdd(size, calculator->getCords(index), isBuilding);
}

std::optional<Urho3D::Vector2> Environment::tryGetValidPosition(const Urho3D::UCharVector2& size,
	                                                            const Urho3D::UShortVector2& bucketCords,
	                                                            bool isBuilding) const {
	return mainGrid.tryGetValidPosition(size, bucketCords, isBuilding);
}

Urho3D::Vector2 Environment::getCenter(int index) const {
	return calculator->getCenter(index);
}

bool Environment::cellInState(int index, CellState state) const {
	return mainGrid.cellInState(index, state);
}

void Environment::incCell(int index, CellState cellState) const {
	assert(calculator->isValidIndex(index));
	mainGrid.incCell(index, cellState);
}

void Environment::decCell(int index) const {
	assert(calculator->isValidIndex(index));
	mainGrid.decCell(index);
}

void Environment::removeFromGrids(std::span<Unit* const> units) {
	for (const auto unit : units) {
		mainGrid.removeAt(unit->getMainGridIndex(), unit);
		sparseUnitGrid.removeAt(unit->getSparseIndex(), unit);
	}
	invalidateCaches();
}

void Environment::removeFromGrids(std::span<Building* const> buildingsToDispose,
                                  std::span<ResourceEntity* const> resourcesToDispose) {
	for (const auto building : buildingsToDispose) {
		mainGrid.removeStatic(building);
		mainGrid.removeDeploy(building);
		buildingGrid.remove(building);
	}

	for (const auto resource : resourcesToDispose) {
		mainGrid.removeStatic(resource);
		resourceStaticGrid.remove(resource);
	}
	if (!resourcesToDispose.empty() || !buildingsToDispose.empty()) {
		mainGrid.invalidatePathCache();
	}
}

unsigned char Environment::getRevertCloseIndex(int center, int gridIndex) const {
	return mainGrid.getRevertCloseIndex(center, gridIndex);
}

void Environment::drawDebug(EnvironmentDebugMode environmentDebugMode, char index) {
	if (environmentDebugMode == EnvironmentDebugMode::MAIN_GRID) {
		mainGrid.drawDebug(GridDebugType(index % magic_enum::enum_count<GridDebugType>()));
		return;
	}
	influenceManager.draw(environmentDebugMode, index);
}

const std::vector<short>& Environment::getCloseIndexs(int center) const {
	return mainGrid.getCloseIndexes(center);
}

std::optional<Urho3D::Vector2> Environment::getPosFromIndexes(const Urho3D::UCharVector2& buildingSize,
                                                              unsigned char player,
                                                              std::span<const unsigned> indexes) {
	const float ratio = influenceManager.getFieldSize() / mainGrid.getFieldSize();
	for (const auto centerIndex : indexes) {
		Urho3D::Vector2 center = influenceManager.getCenter(centerIndex);
		for (const auto index : mainGrid.getCloseCenters(center, ratio)) {
			//ten index jest widoczny
			auto gridCenter = calculator->getCenter(index);
			if (validateStatic(buildingSize, gridCenter, true) &&
				influenceManager.isVisible(player, gridCenter)) {
				return gridCenter;
			}
		}
	}
	return {};
}

std::optional<Urho3D::Vector2> Environment::getPosToCreate(std::span<const float> result,
                                                           const Urho3D::UCharVector2& buildingSize,
                                                           unsigned char player) {
	auto &indexes = influenceManager.getAreas(result, player);

	return getPosFromIndexes(buildingSize, player, indexes);
}

void Environment::addCollect(Unit* unit, short resId, float value) const {
	influenceManager.addCollect(unit, resId, value);
}

void Environment::addAttack(char player, const Urho3D::Vector2& position, float value) const {
	influenceManager.addAttack(player, position, value);
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

bool Environment::cellIsAttackable(int index) const {
	return mainGrid.cellIsAttackable(index);
}

std::optional<Urho3D::Vector2> Environment::getCenterOf(CenterType type, unsigned char player) const {
	return influenceManager.getCenterOf(type, player);
}

float Environment::getDiffOfCenters(CenterType type1, char id1, CenterType type2, char id2,
                                    float defaultVal) const {
	const auto optCenter1 = getCenterOf(type1, id1);
	if (!optCenter1.has_value()) { return defaultVal; }
	const auto optCenter2 = getCenterOf(type2, id2);
	if (!optCenter2.has_value()) { return defaultVal; }

	return (optCenter1.value() - optCenter2.value()).Length()
		/ (calculator->getFieldSize() * calculator->getResolution());
}

bool Environment::anyCloseEnough(std::span<const int> indexes, int center, float distThreshold) const {
	return mainGrid.anyCloseEnough(indexes, center, distThreshold);
}

bool Environment::isInLocalArea(int getMainCell, int aimIndex) const {
	return mainGrid.isInLocalArea(getMainCell, aimIndex);
}

bool Environment::isInLocal1and2Area(int getMainCell, int aimIndex) const {
	return mainGrid.isInLocal1and2Area(getMainCell, aimIndex);
}

int Environment::closestPassableCell(int posIndex) const {
	return mainGrid.closestPassableCell(posIndex);
}

Urho3D::Vector3 Environment::getValidPosition(const Urho3D::UCharVector2& size, const Urho3D::Vector2& pos) const {
	return getValidPosition(size, calculator->getCords(pos));
}

Urho3D::Vector3 Environment::getValidPosition(const Urho3D::UCharVector2& size,
                                              const Urho3D::UShortVector2& bucketCords) const {
	auto pos2d = mainGrid.getValidPosition(size, bucketCords);
	return getPosWithHeightAt(pos2d.x_, pos2d.y_);
}

const std::vector<int>* Environment::findPath(int startIdx, const Urho3D::Vector2& aim) {
	const auto end = calculator->indexFromPosition(aim);

	return mainGrid.findPath(startIdx, end);
}

const std::vector<int>* Environment::findPath(int startIdx, std::span<const int> endIndexes) {
	return mainGrid.findPath(startIdx, endIndexes, true);
}

const std::vector<int>* Environment::findPath(int startIdx, int endIdx) {
	return mainGrid.findPath(startIdx, endIdx);
}

void Environment::prepareGridToFind() const {
	mainGrid.prepareGridToFind();
}

content_info* Environment::getContentInfo(Urho3D::Vector2 centerPercent, bool checks[], int activePlayer) {
	const float x = getPosFromPercent(centerPercent.x_);
	const float z = getPosFromPercent(centerPercent.y_);
	const CellState state = mainGrid.getCellAt(x, z);
	const int addInfo = mainGrid.getAdditionalInfoAt(x, z);
	return influenceManager.getContentInfo({x, z}, state, addInfo, checks, activePlayer);
}

float Environment::getPosFromPercent(float value) const {
	return mapSize * (value - 0.5);
}

Urho3D::Vector2 Environment::getPosFromPercent(float x, float z) const {
	return {getPosFromPercent(x), getPosFromPercent(z)};
}

Physical* Environment::closestPhysical(int startIdx, const std::vector<Physical*>& things,
                                       const std::function<bool(Physical*)>& condition, bool closeEnough) {
	if (things.empty()) {
		return nullptr;
	}
	std::vector<int> allIndexes;
	allIndexes.reserve(things.size());
	std::vector<Physical*> thingsFiltered;
	thingsFiltered.reserve(things.size());

	for (const auto entity : things) {
		if (entity->isAlive() && condition(entity)) {
			//TODO perf ogranizcyc liczbe indeksow, np wybrac jeden dla obiektu
			entity->addIndexesForUse(allIndexes);
			thingsFiltered.push_back(entity);
		}
	}

	if (!allIndexes.empty()) {
		const auto path = mainGrid.findPath(startIdx, allIndexes, closeEnough);
		if (!path->empty()) {
			for (const auto entity : thingsFiltered) {
				if (entity->indexCanBeUse(path->back())) {
					return entity;
				}
			}
		}
	}
	return nullptr;
}

Physical* Environment::closestPhysicalSimple(const Urho3D::Vector2& center, const std::vector<Physical*>& things,
                                             float range) const {
	if (things.empty()) {
		return nullptr;
	}
	range *= range;
	float closestDist = 9999999;
	Physical* closest = nullptr;
	for (const auto entity : things) {
		const float dist = center.SqDistXZ(entity->getPosition());
		if (dist < closestDist && dist <= range) {
			closestDist = dist;
			closest = entity;
		}
	}
	return closest;
}
