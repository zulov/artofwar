﻿#include "Environment.h"

#include <Urho3D/Graphics/Material.h>
#include <Urho3D/Graphics/Terrain.h>
#include "EnvConsts.h"
#include "GridCalculator.h"
#include "GridCalculatorProvider.h"
#include "database/db_strcut.h"
#include "math/MathUtils.h"
#include "objects/building/Building.h"
#include "objects/resource/ResourceEntity.h"
#include "objects/unit/Unit.h"
#include "utils/consts.h"
#include "utils/OtherUtils.h"


Environment::Environment(Urho3D::Terrain* terrain, unsigned short mainMapResolution):
	mapSize(mainMapResolution * BUCKET_GRID_FIELD_SIZE),
	mainGrid(mainMapResolution, mapSize, 24),
	buildingGrid(mapSize / BUCKET_GRID_FIELD_SIZE_BUILD, mapSize, 256.f),
	resourceStaticGrid(mapSize / BUCKET_GRID_FIELD_SIZE_RESOURCE, mapSize, {64.f, 128.f, 256.f}),
	sparseUnitGrid((short)(mapSize / BUCKET_GRID_FIELD_SIZE_ENEMY), mapSize, 256.f),
	terrain(terrain), influenceManager(MAX_PLAYERS, mapSize, terrain),
	calculator(GridCalculatorProvider::get(mainMapResolution, mapSize)) {
	neights = new std::vector<Physical*>();

	auto a = {160, 192, 256, 320, 384, 512};
	assert(std::ranges::any_of(a, [mainMapResolution](int i) {return mainMapResolution == i; }));
}


Environment::~Environment() {
	delete neights;
}

std::vector<Physical*>* Environment::getNeighboursFromSparseSamePlayer(Physical* physical, const float radius,
                                                                       char player) {
	return getNeighbours(physical, sparseUnitGrid, radius,
	                     [player](const Physical* thing) { return thing->getPlayer() == player && thing->isAlive(); });
}

std::vector<Physical*>* Environment::getNeighboursFromTeamNotEq(Physical* physical, const float radius) {
	auto player = physical->getPlayer();

	return getNeighbours(physical, sparseUnitGrid, radius,
	                     [player](const Physical* thing) { return thing->getPlayer() != player && thing->isAlive(); });
}

bool Environment::isVisible(char player, const Urho3D::Vector2& pos) const {
	return influenceManager.isVisible(player, pos);
}

float Environment::getVisibilityScore(char player) const {
	return influenceManager.getVisibilityScore(player);
}

std::vector<int> Environment::getIndexesInRange(const Urho3D::Vector3& center, float range) const {
	return mainGrid.getIndexesInRange(center, range);
}

void Environment::setTerrainShaderParam(const Urho3D::String& name, const Urho3D::Variant& value) const {
	if (terrain && !SIM_GLOBALS.HEADLESS) {
		terrain->GetMaterial()->SetShaderParameter(name, value);
	}
}

void Environment::flipTerrainShaderParam(const Urho3D::String& name) const {
	if (terrain && !SIM_GLOBALS.HEADLESS) {
		auto mat = terrain->GetMaterial();

		mat->SetShaderParameter(name, !mat->GetShaderParameter(name).GetBool());
	}
}

void Environment::nextVisibilityType() {
	influenceManager.nextVisibilityType();
}

void Environment::reAddBonuses(std::vector<Building*>* buildings, char player, char resId) const {
	mainGrid.reAddBonuses(buildings, player, resId);
}

float Environment::getBonuses(char player, const ResourceEntity* resource) const {
	return mainGrid.getBonuses(player, resource);
}

void Environment::refreshAllStatic(std::vector<int>& indexes) {
	mainGrid.refreshAllStatic(std::span(indexes.data(), indexes.size()));
}

std::vector<Physical*>* Environment::getNeighbours(Physical* physical, Grid& bucketGrid, float radius,
                                                   const std::function<bool(Physical*)>& condition) const {
	neights->clear();
	BucketIterator& bucketIterator = bucketGrid.getArrayNeight(physical->getPosition(), radius);
	const float sqRadius = radius * radius;

	while (Physical* neight = bucketIterator.next()) {
		//if bucket wystarczajaco blisk ododaj bez sprawdzania odleglosci
		addIfInRange(physical, neight, sqRadius, condition);
	}

	return neights;
}

void Environment::addIfInRange(Physical* physical, Physical* neight, const float sqRadius,
                               const std::function<bool(Physical*)>& condition) const {
	if (physical != neight && condition(neight)
		&& sqDistAs2D(physical->getPosition(), neight->getPosition()) < sqRadius) {
		neights->push_back(neight);
	}
}

std::vector<Physical*>* Environment::getNeighboursWithCache(Unit* unit, float radius) {
	const auto currentIdx = unit->getMainGridIndex();
	assert(currentIdx>=0);
	if (mainGrid.onlyOneInside(currentIdx)) {
		return getNeighbours(unit, mainGrid, radius, [](Physical* physical) { return true; });
	}
	const auto simpleNeght = mainGrid.getAllFromCache(currentIdx, radius);

	const float sqRadius = radius * radius;

	neights->clear();
	auto pred = [sqRadius,unit](Physical* neight) {
		return (unit != neight && sqDistAs2D(unit->getPosition(), neight->getPosition()) < sqRadius);
	};
	std::ranges::copy_if(*simpleNeght, std::back_inserter(*neights), pred);

	return neights;
}

std::vector<Physical*>* Environment::getNeighbours(const Urho3D::Vector3& center, Grid& bucketGrid, int id,
                                                   float radius,
                                                   float prevRadius) const {
	neights->clear();

	BucketIterator& bucketIterator = bucketGrid.getArrayNeight(center, radius);
	const float sqRadius = radius * radius;
	const float sqPrevRadius = prevRadius < 0.f ? prevRadius : prevRadius * prevRadius;

	while (Physical* neight = bucketIterator.next()) {
		if (id == -1 || id == neight->getId()) {
			auto dist = sqDistAs2D(center, neight->getPosition());
			if (dist <= sqRadius && dist > sqPrevRadius) {
				neights->push_back(neight);
			}
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

std::vector<Physical*>*
Environment::getResources(const Urho3D::Vector3& center, int id, float radius, float prevRadius) {
	//return getNeighbours(center, resourceStaticGrid, id, radius, prevRadius);//TODO perf? czy to moze jedna szybsze?
	const float sqRadius = radius * radius;
	const float sqPrevRadius = prevRadius < 0.f ? prevRadius : prevRadius * prevRadius;
	neights->clear();
	for (auto neight : resourceStaticGrid.get(center, radius)) {
		if (id == -1 || id == neight->getId()) {
			auto dist = sqDistAs2D(center, neight->getPosition());
			if (dist <= sqRadius && dist > sqPrevRadius) {
				neights->push_back(neight);
			}
		}
	}

	return neights;
}

std::vector<Physical*>* Environment::getResources(const Urho3D::Vector3& center, int id, float radius) {
	return getNeighbours(center, resourceStaticGrid, id, radius, -1);
}

std::vector<Physical*>*
Environment::getBuildingsFromTeamNotEq(Physical* physical, int id, float radius) {
	auto team = physical->getTeam();
	auto condition = [id, team](const Physical* physical) {
		return (id < 0 || physical->getId() == id) && (physical->getTeam() != team || team < 0);
	};
	return getNeighbours(physical, buildingGrid, radius, condition);
}

void Environment::updateInfluenceUnits1(std::vector<Unit*>* units) {
	influenceManager.update(units);
}

void Environment::updateInfluenceUnits2(std::vector<Unit*>* units) {
	influenceManager.updateQuadUnits(units);
}

void Environment::updateInfluenceResources(std::vector<ResourceEntity*>* resources) const {
	influenceManager.update(resources);
}

void Environment::updateInfluenceOther(std::vector<Building*>* buildings, std::vector<Unit*>* units) const {
	influenceManager.update(buildings);
	influenceManager.updateWithHistory();
	influenceManager.updateNotInBonus(units);
}

void Environment::updateQuadOther() const {
	influenceManager.updateQuadOther();
}

void Environment::updateVisibility(std::vector<Building*>* buildings, std::vector<Unit*>* units,
                                   std::vector<ResourceEntity*>* resources) const {
	influenceManager.updateVisibility(buildings, units, resources);
}

void Environment::updateInfluenceHistoryReset() const {
	influenceManager.updateInfluenceHistoryReset();
}

void Environment::update(Unit* unit) const {
	unit->setBucketInMainGrid(mainGrid.update(unit, unit->getMainGridIndex(), true));
	if (unit->indexChanged()) {
		unit->setSparseIndex(sparseUnitGrid.update(unit, unit->getSparseIndex(), false));
		unit->setIndexInInfluence(-1);
	}
}

void Environment::addNew(const std::vector<Unit*>& units) {
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
	mainGrid.addResourceBonuses(building);
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
}

void Environment::addNew(ResourceEntity* resource, bool bulkAdd) {
	mainGrid.addStatic(resource, bulkAdd);
	resource->setIndexInInfluence(influenceManager.getIndex(resource->getPosition()));
	resourceStaticGrid.updateStatic(resource, bulkAdd);
}

Urho3D::Vector2 Environment::repulseObstacle(Unit* unit) const {
	return mainGrid.repulseObstacle(unit);
}

std::optional<Urho3D::Vector2> Environment::validatePosition(int index, const Urho3D::Vector3& position) const {
	return mainGrid.getDirectionFrom(index, position);
}

const std::vector<Physical*>* Environment::getNeighbours(std::pair<Urho3D::Vector3*, Urho3D::Vector3*>& pair,
                                                         char player) {
	for (const auto grid : grids) {
		const auto result = grid->getArrayNeight(pair, player);
		if (!result->empty()) {
			return result;
		}
	}
	return &Consts::EMPTY_PHYSICAL;
}

float Environment::getGroundHeightAt(float x, float z) const {
	const auto vec = Urho3D::Vector3(x, 0, z);
	return getGroundHeightAt(vec);
}

float Environment::getGroundHeightAt(const Urho3D::Vector3& pos) const {
	if (terrain != nullptr) {
		return terrain->GetHeight(pos);
	}
	return 0.f;
}

Urho3D::Vector3 Environment::getPosWithHeightAt(float x, float z) const {
	return Urho3D::Vector3(x, getGroundHeightAt(x, z), z);
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

bool Environment::validateStatic(const Urho3D::IntVector2& size, Urho3D::Vector2& pos, bool isBuilding) const {
	return mainGrid.validateAdd(size, {calculator->getIndex(pos.x_), calculator->getIndex(pos.y_)}, isBuilding);
}

bool Environment::validateStatic(const Urho3D::IntVector2& size, const Urho3D::IntVector2 bucketCords,
                                 bool isBuilding) const {
	return mainGrid.validateAdd(size, bucketCords, isBuilding);
}

Urho3D::Vector2 Environment::getCenter(int index) const {
	return calculator->getCenter(index);
}

Urho3D::Vector2 Environment::getCenter(short x, short z) const {
	return getCenter(calculator->getIndex(x, z));
}

void Environment::invalidatePathCache() {
	mainGrid.invalidatePathCache();
}

bool Environment::cellInState(int index, CellState state) const {
	return mainGrid.cellInState(index, state);
}

void Environment::updateCell(int index, char val, CellState cellState) const {
	mainGrid.updateCell(index, val, cellState);
}

void Environment::removeFromGrids(const std::vector<Unit*>& units) const {
	for (const auto unit : units) {
		mainGrid.removeAt(unit->getMainGridIndex(), unit);
		sparseUnitGrid.removeAt(unit->getSparseIndex(), unit);
	}
}

void Environment::removeFromGrids(const std::vector<Building*>& buildingsToDispose,
                                  const std::vector<ResourceEntity*>& resourceToDispose) const {
	for (const auto building : buildingsToDispose) {
		mainGrid.removeStatic(building);
		mainGrid.removeDeploy(building);
		buildingGrid.remove(building);
	}

	for (const auto resource : resourceToDispose) {
		mainGrid.removeStatic(resource);
		resourceStaticGrid.remove(resource);
	}
}

unsigned char Environment::getRevertCloseIndex(int center, int gridIndex) const {
	return mainGrid.getRevertCloseIndex(center, gridIndex);
}

void Environment::drawDebug(EnvironmentDebugMode environmentDebugMode, char index) {
	if (environmentDebugMode == EnvironmentDebugMode::MAIN_GRID) {
		mainGrid.drawDebug(GridDebugType(index % magic_enum::enum_count<GridDebugType>()));
	}
	influenceManager.draw(ENV_TO_INF_MAP[cast(environmentDebugMode)], index);
}

const std::vector<short>& Environment::getCloseIndexs(int center) const {
	return mainGrid.getCloseIndexes(center);
}

std::array<float, 5>& Environment::getInfluenceDataAt(char player, const Urho3D::Vector2& pos) {
	return influenceManager.getInfluenceDataAt(player, pos);
}

std::optional<Urho3D::Vector2> Environment::getPosFromIndexes(db_building* building, char player,
                                                              const std::vector<unsigned>* indexes) {
	const float ratio = influenceManager.getFieldSize() / mainGrid.getFieldSize();
	for (const auto centerIndex : *indexes) {
		Urho3D::Vector2 center = influenceManager.getCenter(centerIndex);
		for (const auto index : mainGrid.getCloseCenters(center, ratio)) {
			//ten index jest widoczny
			auto gridCenter = calculator->getCenter(index);
			if (validateStatic(building->size, gridCenter, true) && influenceManager.isVisible(player, gridCenter)) {
				return gridCenter;
			}
		}
	}
	return {};
}

std::optional<Urho3D::Vector2> Environment::getPosToCreate(const std::span<float> result, ParentBuildingType type,
                                                           db_building* building,
                                                           char player) {
	const std::vector<unsigned>* indexes = influenceManager.getAreas(result, type, player);

	return getPosFromIndexes(building, player, indexes);
}

std::optional<Urho3D::Vector2> Environment::getPosToCreateResBonus(db_building* building, char player) {
	std::vector<unsigned> allIndexes;
	for (const char id : building->resourceTypes) {
		auto indexes = influenceManager.getAreasResBonus(id, player);
		//TODO  jak bedzie kilka resourceTypes to trzeba by to brac po koleji dla największch wartosci niezelzenie od tego jaki jest resource
		allIndexes.insert(allIndexes.end(), indexes.begin(), indexes.end());
	}

	return getPosFromIndexes(building, player, &allIndexes);
}

std::vector<Urho3D::Vector2> Environment::getAreas(char player, const std::span<float> result, int min) {
	return influenceManager.getAreasIterative(result, player, 0.1, min);
}

void Environment::addCollect(Unit* unit, short resId, float value) {
	influenceManager.addCollect(unit, resId, value);
}

void Environment::addAttack(char player, const Urho3D::Vector3& position, float value) {
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

std::optional<Urho3D::Vector2> Environment::getCenterOf(CenterType type, char player) {
	return influenceManager.getCenterOf(type, player);
}

float Environment::getDiffOfCenters(CenterType type1, char id1, CenterType type2, char id2, float dfVal) {
	const auto optCenter1 = getCenterOf(type1, id1);
	if (!optCenter1.has_value()) { return dfVal; }
	const auto optCenter2 = getCenterOf(type2, id2);
	if (!optCenter2.has_value()) { return dfVal; }

	return (optCenter1.value() - optCenter2.value()).Length()
		/ (calculator->getFieldSize() * calculator->getResolution());
}

bool Environment::anyCloseEnough(std::vector<int> const& indexes, int center, float distThreshold) const {
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

Urho3D::Vector2 Environment::getValidPosition(const Urho3D::IntVector2& size, const Urho3D::Vector2& pos) const {
	return mainGrid.getValidPosition(size, pos);
}

Urho3D::Vector2 Environment::getValidPosition(const Urho3D::IntVector2& size,
                                              const Urho3D::IntVector2& bucketCords) const {
	return mainGrid.getValidPosition(size, getCenter(calculator->getIndex(bucketCords.x_, bucketCords.y_)));
}

const std::vector<int>* Environment::findPath(int startIdx, const Urho3D::Vector2& aim) {
	auto end = calculator->indexFromPosition(aim);
	auto dist = calculator->getBiggestDiff(startIdx, end);
	dist = dist < 9 ? 9 : dist;
	return mainGrid.findPath(startIdx, end, dist * sqrt(dist));
}

const std::vector<int>* Environment::findPath(int startIdx, const std::vector<int>& endIdxs) {
	auto dist = calculator->getBiggestDiff(startIdx, endIdxs);
	dist = dist < 9 ? 9 : dist;
	return mainGrid.findPath(startIdx, endIdxs, dist * sqrt(dist), true);
}

const std::vector<int>* Environment::findPath(int startIdx, int endIdx, int limit) {
	return mainGrid.findPath(startIdx, endIdx, limit);
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

Physical* Environment::closestPhysical(Unit* unit, const std::vector<Physical*>* things,
                                       const std::function<bool(Physical*)>& condition, int limit, bool closeEnough) {
	if (things->empty()) {
		return nullptr;
	}
	std::vector<int> allIndexes;
	std::unordered_map<int, Physical*> idxToPhysical;

	for (const auto entity : *things) {
		if (entity->isAlive() && condition(entity)) {
			auto const idxs = entity->getIndexesForUse(unit);
			//TODO perf ogranizcyc liczbe indeksów, np wybrac jeden dla obiektu
			allIndexes.insert(allIndexes.end(), idxs.begin(), idxs.end());
			for (auto idx : idxs) {
				idxToPhysical[idx] = entity;
			}
		}
	}

	if (!allIndexes.empty()) {
		const auto path = mainGrid.findPath(unit->getMainGridIndex(), allIndexes, limit, false);
		if (!path->empty()) {
			return idxToPhysical[path->back()];
		}
	}
	return nullptr;
}

Physical* Environment::closestPhysicalSimple(const Physical* physical, const std::vector<Physical*>* things,
                                             float range) const {
	if (things->empty()) {
		return nullptr;
	}
	range *= range;
	float closestDist = 9999999;
	Physical* closest = nullptr;
	const auto center = physical->getPosition();
	for (const auto entity : *things) {
		const float dist = sqDistAs2D(center, entity->getPosition());
		if (dist < closestDist && dist <= range) {
			closestDist = dist;
			closest = entity;
		}
	}
	return closest;
}