#include "Environment.h"

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


Environment::Environment(Urho3D::Terrain* terrain, unsigned short mainMapResolution):
	mapSize(mainMapResolution * BUCKET_GRID_FIELD_SIZE),
	mainGrid(mainMapResolution, mapSize, 24),
	resourceStaticGrid(mapSize / BUCKET_GRID_FIELD_SIZE_RESOURCE, mapSize, {64.f, 128.f, 256.f}),
	buildingGrid(mapSize / BUCKET_GRID_FIELD_SIZE_BUILD, mapSize, false, 256.f),
	sparseUnitGrid((short)(mapSize / BUCKET_GRID_FIELD_SIZE_ENEMY), mapSize, false, 256.f),
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

float Environment::getVisibilityScore(char player) {
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

std::vector<Physical*>* Environment::getNeighbours(Physical* physical, Grid& bucketGrid, float radius,
                                                   const std::function<bool(Physical*)>& condition) const {
	neights->clear();
	BucketIterator& bucketIterator = bucketGrid.getArrayNeight(physical->getPosition(), radius);
	const float sqRadius = radius * radius;

	while (Physical* neight = bucketIterator.next()) {
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
	influenceManager.updateWithHistory();
}

void Environment::updateQuadOther() const {
	influenceManager.updateQuadOther();
}

void Environment::updateVisibility(std::vector<Building*>* buildings, std::vector<Unit*>* units,
                                   std::vector<ResourceEntity*>* resources) const {
	influenceManager.updateVisibility(buildings, units, resources);
}

void Environment::update(Unit* unit) const {
	unit->setBucket(mainGrid.update(unit, unit->getMainGridIndex()));
	unit->setSparseIndex(sparseUnitGrid.update(unit, unit->getSparseIndex()));
}

void Environment::addNew(const std::vector<Unit*>& units) {
	for (const auto unit : units) {
		assert(unit->getMainGridIndex() == -1);
		unit->setBucket(mainGrid.updateNew(unit));
		unit->setSparseIndex(sparseUnitGrid.updateNew(unit));
	}

	invalidateCaches();
}

void Environment::invalidateCaches() {
	mainGrid.invalidateCache();
	sparseUnitGrid.invalidateCache();
}

void Environment::addNew(Building* building) const {
	mainGrid.addStatic(building);
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

void Environment::addNew(ResourceEntity* resource, bool bulkAdd) const {
	mainGrid.addStatic(resource);
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

float Environment::getGroundHeightPercent(float y, float x, float div) const {
	if (terrain != nullptr) {
		const float scale = terrain->GetSpacing().y_;
		auto const a = Urho3D::Vector3(x * mapSize - mapSize * 0.5f, 0.f,
		                               y * mapSize - mapSize * 0.5f);

		return getGroundHeightAt(a) / scale / div;
	}
	return 0.f;
}

Urho3D::Vector3 Environment::getValidPosForCamera(float percentX, float percentY, const Urho3D::Vector3& pos,
                                                  float min) const {
	auto a = Urho3D::Vector3(percentX * mapSize - mapSize * 0.5f, pos.y_,
	                         percentY * mapSize - mapSize * 0.5f);
	const float h = getGroundHeightAt(a);
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
	return calculator->getCenter(index);
}

Urho3D::Vector2 Environment::getCenter(short x, short z) const {
	return getCenter(calculator->getIndex(x, z));
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

const std::vector<unsigned char>& Environment::getCloseTabIndexes(int center) const {
	return mainGrid.getCloseTabIndexes(center);
}

std::array<float, 5>& Environment::getInfluenceDataAt(char player, const Urho3D::Vector2& pos) {
	return influenceManager.getInfluenceDataAt(player, pos);
}

std::optional<Urho3D::Vector2> Environment::getPosToCreate(db_building* building, char player,
                                                           const std::span<float> result) {
	std::vector<int>* indexes = influenceManager.getAreas(result, player);

	const float ratio = influenceManager.getFieldSize() / mainGrid.getFieldSize();
	for (const auto centerIndex : *indexes) {
		Urho3D::Vector2 center = influenceManager.getCenter(centerIndex);
		for (const auto index : mainGrid.getCloseCenters(center, ratio)) {
			//ten index jest widoczny
			auto gridCenter = calculator->getCenter(index);
			if (validateStatic(building->size, gridCenter) && influenceManager.isVisible(player, gridCenter)) {
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

std::optional<Urho3D::Vector2> Environment::getCenterOf(CenterType id, char player) {
	return influenceManager.getCenterOf(id, player);
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
	return mapSize * (value - 0.5);
}

Physical* Environment::closestPhysical(Unit* unit, const std::vector<Physical*>* things,
                                       const std::function<bool(Physical*)>& condition, int limit) const {
	if (things->empty()) {
		return nullptr;
	}
	std::vector<int> allIndexes;
	std::unordered_map<int, Physical*> idxToPhysical;

	for (const auto entity : *things) {
		if (entity->isAlive() && condition(entity)) {
			auto const idxs = entity->getIndexesForUse(unit);
			allIndexes.insert(allIndexes.end(), idxs.begin(), idxs.end());
			for (auto idx : idxs) {
				idxToPhysical[idx] = entity;
			}
		}
	}
	if (!allIndexes.empty()) {
		const auto path = mainGrid.findPath(unit->getMainGridIndex(), allIndexes, limit);
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
