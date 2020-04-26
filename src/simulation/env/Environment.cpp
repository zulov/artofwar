#include "Environment.h"
#include "objects/ObjectEnums.h"
#include "objects/building/Building.h"
#include "objects/resource/ResourceEntity.h"
#include "objects/unit/Unit.h"
#include <array>
#include <chrono>
#include <simulation/env/bucket/BucketIterator.h>
#include "EnvConsts.h"


Environment::Environment(Urho3D::Terrain* terrian):
	mainGrid(BUCKET_GRID_RESOLUTION, BUCKET_GRID_SIZE),
	resourceGrid(BUCKET_GRID_RESOLUTION_RESOURCE, BUCKET_GRID_SIZE),
	buildingGrid(BUCKET_GRID_RESOLUTION_BUILD, BUCKET_GRID_SIZE),
	teamUnitGrid{
		{BUCKET_GRID_RESOLUTION_ENEMY, BUCKET_GRID_SIZE},
		{BUCKET_GRID_RESOLUTION_ENEMY, BUCKET_GRID_SIZE}
	}, influenceManager(MAX_PLAYERS), terrian(terrian) {
	neights = new std::vector<Physical*>();
	neights2 = new std::vector<Physical*>();
	empty = new std::vector<Physical*>();
	neights->reserve(DEFAULT_VECTOR_SIZE * 2);
	neights2->reserve(DEFAULT_VECTOR_SIZE * 2);
}


Environment::~Environment() {
	delete neights;
	delete neights2;
	delete empty;
}

std::vector<Physical*>* Environment::getNeighbours(Physical* physical, const float radius) {
	return getNeighbours(physical, mainGrid, radius);
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

	const auto center = physical->getPosition();
	BucketIterator& bucketIterator = bucketGrid.getArrayNeight(physical->getPosition(), radius, 0);
	const float sqRadius = radius * radius;

	while (Physical* neight = bucketIterator.next()) {
		if (physical == neight) { continue; }
		const float xDiff = center.x_ - neight->getPosition().x_;
		const float zDiff = center.z_ - neight->getPosition().z_;

		if (xDiff * xDiff + zDiff * zDiff < sqRadius) {
			neights->push_back(neight);
		}
	}

	return neights;
}

std::vector<Physical*>* Environment::getNeighboursSimilarAs(Physical* clicked) const {
	Grid* grid;
	switch (clicked->getType()) {
	case ObjectType::UNIT:
		grid = grids[0];
		break;
	case ObjectType::BUILDING:
		grid = grids[1];
		break;
	case ObjectType::RESOURCE:
		grid = grids[2];
		break;
	default: return empty;
	}
	return grid->getArrayNeightSimilarAs(clicked, 20.0);
}

std::vector<Physical*>* Environment::getResources(Physical* physical, float radius) {
	return getNeighbours(physical, resourceGrid, radius);
}

void Environment::updateInfluence(std::vector<Unit*>* units,
                                  std::vector<Building*>* buildings,
                                  std::vector<ResourceEntity*>* resources) const {
	influenceManager.update(units);
	influenceManager.update(buildings);
	influenceManager.update(resources);

	influenceManager.update(units, buildings);
}

void Environment::update(std::vector<Unit*>* units) const {
	//TODO to mozna rodzielic na dodawanei u usywanie
	for (auto unit : *units) {
		mainGrid.update(unit);
		teamUnitGrid[unit->getTeam()].update(unit, unit->getTeam());
	}
}

void Environment::update(std::vector<Building*>* buildings) {
	for (auto building : *buildings) {
		mainGrid.addStatic(building);
		buildingGrid.update(building);
		building->createDeploy();

		mainGrid.addDeploy(building);
	}
}

void Environment::updateAll(std::vector<Building*>* const buildings) const {
	//TODO performance, zmianiac tylko to co sie zmienilo
	influenceManager.update(buildings); //TODO bug updej tylko jak sie doda a nie uwzglednia usuwania
}

void Environment::update(std::vector<ResourceEntity*>* resources) {
	for (auto resource : *resources) {
		mainGrid.addStatic(resource);
		resourceGrid.update(resource);
	}
}

Urho3D::Vector2 Environment::repulseObstacle(Unit* unit) {
	return mainGrid.repulseObstacle(unit);
}

Urho3D::Vector2* Environment::validatePosition(Urho3D::Vector3& position) {
	return mainGrid.getDirectionFrom(position);
}

std::vector<Physical*>* Environment::getNeighbours(std::pair<Urho3D::Vector3*, Urho3D::Vector3*>& pair, char player) {
	for (auto grid : grids) {
		const auto result = grid->getArrayNeight(pair, player);
		if (!result->empty()) {
			return result;
		}
	}
	return empty;
}

float Environment::getGroundHeightAt(float x, float z) const {
	return terrian->GetHeight(Urho3D::Vector3(x, 0, z));
}

Urho3D::Vector3 Environment::getPosWithHeightAt(float x, float z) const {
	return Urho3D::Vector3(x, getGroundHeightAt(x, z), z);
}

Urho3D::Vector3 Environment::getPosWithHeightAt(int index) const {
	const auto center = mainGrid.getCenter(index);
	return getPosWithHeightAt(center.x_, center.y_);
}

float Environment::getGroundHeightPercent(float y, float x, float div) const {
	const float scale = terrian->GetSpacing().y_;
	const auto a = Urho3D::Vector3(x * BUCKET_GRID_SIZE - BUCKET_GRID_SIZE * 0.5, 0,
	                               y * BUCKET_GRID_SIZE - BUCKET_GRID_SIZE * 0.5);

	return terrian->GetHeight(a) / scale / div;
}

Urho3D::Vector3 Environment::getValidPosForCamera(float percentX, float percentY, const Urho3D::Vector3& pos,
                                                  float min) const {
	auto a = Urho3D::Vector3(percentX * BUCKET_GRID_SIZE - BUCKET_GRID_SIZE * 0.5, pos.y_,
	                         percentY * BUCKET_GRID_SIZE - BUCKET_GRID_SIZE * 0.5);
	const float h = terrian->GetHeight(a);
	if (h + min > pos.y_) {
		a.y_ = h + min;
	}
	return a;
}

bool Environment::validateStatic(const Urho3D::IntVector2& size, Urho3D::Vector2& pos) const {
	return mainGrid.validateAdd(size, pos);
}

Urho3D::Vector2 Environment::getValidPosition(const Urho3D::IntVector2& size,
                                              const Urho3D::IntVector2& bucketCords) const {
	return getValidPosition(size, mainGrid.getCenterAt(bucketCords)).second;
}

Urho3D::Vector2& Environment::getCenter(int index) const {
	return mainGrid.getCenter(index);
}

Urho3D::Vector2& Environment::getCenter(short x, short z) const {
	return mainGrid.getCenter(x, z);
}

Urho3D::Vector2 Environment::getPositionInBucket(int index, char max, char i) {
	return mainGrid.getPositionInBucket(index, max, i);
}

void Environment::invalidateCache() const {
	mainGrid.invalidateCache();
}

bool Environment::cellInState(int index, std::vector<CellState> cellStates) const {
	return mainGrid.cellInStates(index, cellStates);
}

void Environment::updateCell(int index, char val, CellState cellState) const {
	mainGrid.updateCell(index, val, cellState);
}

bool Environment::belowCellLimit(int index) const {
	return mainGrid.belowCellLimit(index);
}

char Environment::getNumberInState(int index, UnitState state) const {
	return mainGrid.getNumberInState(index, state);
}

char Environment::getOrdinalInState(Unit* unit, UnitState state) const {
	return mainGrid.getOrdinalInState(unit, state);
}

void Environment::removeFromGrids(const std::vector<Physical*>& toDispose) const {
	for (auto dispose : toDispose) {
		switch (dispose->getType()) {
		case ObjectType::BUILDING:
		{
			const auto building = dynamic_cast<Building*>(dispose);
			mainGrid.removeStatic(building);
			mainGrid.removeDeploy(building);
			buildingGrid.update(dispose);
		}
		break;
		case ObjectType::RESOURCE:
		{
			const auto resource = dynamic_cast<ResourceEntity*>(dispose);
			mainGrid.removeStatic(resource);
			resourceGrid.update(resource);
		}
		break;
		default: ;
		}
	}
}

int Environment::getRevertCloseIndex(int center, int gridIndex) {
	return mainGrid.getRevertCloseIndex(center, gridIndex);
}

void Environment::drawDebug(EnvironmentDebugMode environmentDebugMode, char index) {
	switch (environmentDebugMode) {

	case EnvironmentDebugMode::NONE:
		influenceManager.draw(InfluenceType::NONE, index);
		break;
	case EnvironmentDebugMode::MAIN_GRID:
		mainGrid.drawDebug(GridDebugType(index % magic_enum::enum_count<GridDebugType>()));
		influenceManager.draw(InfluenceType::NONE, index);
		break;
	case EnvironmentDebugMode::INF_UNITS_NUMBER_PER_PLAYER:
		influenceManager.draw(InfluenceType::UNITS_NUMBER_PER_PLAYER, index);
		break;
	case EnvironmentDebugMode::INF_UNITS_INFLUENCE_PER_PLAYER:
		influenceManager.draw(InfluenceType::UNITS_INFLUENCE_PER_PLAYER, index);
		break;
	case EnvironmentDebugMode::INF_BUILDING_INFLUENCE_PER_PLAYER:
		influenceManager.draw(InfluenceType::BUILDING_INFLUENCE_PER_PLAYER, index);
		break;
	case EnvironmentDebugMode::INF_RESOURCE:
		influenceManager.draw(InfluenceType::ECON_INFLUENCE_PER_PLAYER, index);
		break;
	case EnvironmentDebugMode::INF_ATTACK_INFLUENCE_PER_PLAYER:
		influenceManager.draw(InfluenceType::ATTACK_INFLUENCE_PER_PLAYER, index);
		break;
	case EnvironmentDebugMode::INF_DEFENCE_INFLUENCE_PER_PLAYER:
		influenceManager.draw(InfluenceType::DEFENCE_INFLUENCE_PER_PLAYER, index);
		break;
	case EnvironmentDebugMode::INF_ECON_INFLUENCE_PER_PLAYER:
		influenceManager.draw(InfluenceType::ECON_INFLUENCE_PER_PLAYER, index);
		break;
	default: ;
	}

}

float Environment::getDistToEnemy(Player* player) {
	return 500.0; //TODO IMPLEMENT
}

const std::vector<short>& Environment::getCloseIndexs(int center) const {
	return mainGrid.getCloseIndexes(center);
}

const std::vector<char>& Environment::getCloseTabIndexes(int center) const {
	return mainGrid.getCloseTabIndexes(center);
}

void Environment::writeInInfluenceDataAt(float* data, char player, const Urho3D::Vector2& pos) {
	influenceManager.writeInInfluenceDataAt(data, player, pos);
}

std::optional<Urho3D::Vector2> Environment::getPosToCreate(db_building* building, char player, float* result) {
	std::vector<Urho3D::Vector2> centers = influenceManager.getAreas(result, player, 0.1);
	float infSize = influenceManager.getFieldSize();
	float mainSize = mainGrid.getFieldSize();
	float ratio = infSize / mainSize; //TODO use 
	for (auto& center : centers) {
		//TODO bug to jest 3x3 a ma byc 4x4 i jeszcze przesuniete jest
		if (validateStatic(building->size, center)) {
			return center;
		}
		auto centerIndex = mainGrid.getIndex(center);
		for (auto index : mainGrid.getCloseIndexes(centerIndex)) {
			if (validateStatic(building->size, center)) {
				return center;
			}
		}

	}
	return {};
}

bool Environment::isInLocalArea(int getMainCell, Urho3D::Vector2& pos) {
	return mainGrid.isInLocalArea(getMainCell, pos);
}

int Environment::closestEmpty(int posIndex) {
	return mainGrid.closestEmpty(posIndex);
}

std::pair<Urho3D::IntVector2, Urho3D::Vector2> Environment::getValidPosition(
	const Urho3D::IntVector2& size, const Urho3D::Vector2& pos) const {
	return mainGrid.getValidPosition(size, pos);
}

std::vector<int>* Environment::findPath(int startIdx, Urho3D::Vector2& aim) const {
	return mainGrid.findPath(startIdx, aim);
}

std::vector<int>* Environment::findPath(Urho3D::Vector3& from, Urho3D::Vector2& aim) const {
	return mainGrid.findPath(from, aim);
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
	return value * BUCKET_GRID_SIZE - BUCKET_GRID_SIZE * 0.5;
}
