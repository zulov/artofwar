#include "SimulationObjectManager.h"

#include <Urho3D/IO/Log.h>

#include "Game.h"
#include "objects/building/Building.h"
#include "objects/resource/ResourceEntity.h"
#include "objects/unit/Unit.h"
#include "objects/unit/state/StateManager.h"
#include "player/Player.h"
#include "player/PlayersManager.h"
#include "scene/load/dbload_container.h"
#include "env/Environment.h"


SimulationObjectManager::SimulationObjectManager() {
	units = new std::vector<Unit*>();
	buildings = new std::vector<Building*>();
	resources = new std::vector<ResourceEntity*>();

	units->reserve(4069);
	buildings->reserve(128);
	resources->reserve(20 * 1024);
}


SimulationObjectManager::~SimulationObjectManager() {
	clear_and_delete_vector(units);
	clear_and_delete_vector(buildings);
	clear_and_delete_vector(resources);

	dispose();
}

void SimulationObjectManager::clearNodesWithoutDelete() {
	for (auto value : *units) {
		value->clearNodeWithOutDelete();
	}
	for (auto value : *buildings) {
		value->clearNodeWithOutDelete();
	}
	for (auto value : *resources) {
		value->clearNodeWithOutDelete();
	}
}

void SimulationObjectManager::addUnits(unsigned number, int id, Urho3D::Vector2& center, char level, char player) {
	addUnits(unitFactory.create(number, id, center, player, level));
}

void SimulationObjectManager::addBuilding(int id, const Urho3D::IntVector2& _bucketCords, char level,
                                          char player) const {
	auto* building = buildingFactory.create(id, _bucketCords, level, player);
	if (building) {
		building->postCreate();
		addBuilding(building, false);
	}
}

void SimulationObjectManager::addResource(int id, const Urho3D::IntVector2& _bucketCords, char level) const {
	auto res = resourceFactory.create(id, _bucketCords, level);
	if (res) {
		addResource(res, false);
	}
}

void SimulationObjectManager::findToDispose() {
	findToDisposeUnits(); //TODO perf jezeli cos zmieni³o stan do dispose
	findToDisposeBuildings();
	findToDisposeResources();
}

void SimulationObjectManager::load(dbload_unit* unit) {
	addUnits(unitFactory.load(unit));
}

void SimulationObjectManager::load(dbload_building* building) const {
	addBuilding(buildingFactory.load(building), true);
}

void SimulationObjectManager::load(dbload_resource_entities* resource) const {
	addResource(resourceFactory.load(resource), true);
}

void SimulationObjectManager::refreshAllStatic() {
	std::vector<int> allCells;
	allCells.reserve(100000);
	bool arr[256 * 256]= { false };//BUG change size
	for (const auto resource : *resources) {
		for (const int allCell : resource->getAllCells()) {
			arr[allCell] = true;
		}
	}
	for (const auto building : *buildings) {
		for (const int allCell : building->getAllCells()) {
			arr[allCell] = true;
		}
	}

	for (int i = 0; i < 256*256; ++i) {
		if (arr[i]) {
			allCells.push_back(i);
		}
	}
	Game::getEnvironment()->refreshAllStatic(allCells);
}

void SimulationObjectManager::addUnits(std::vector<Unit*>& temp) const {
	if (!temp.empty()) {
		units->insert(units->end(), temp.begin(), temp.end());
		for (auto value : temp) {
			Game::getPlayersMan()->getPlayer(value->getPlayer())->add(value);
		}
		Game::getEnvironment()->addNew(temp);
	}
}


void SimulationObjectManager::addBuilding(Building* building, bool bulkAdd) const {
	if (building) {
		buildings->push_back(building);

		Game::getPlayersMan()->getPlayer(building->getPlayer())->add(building);
		Game::getEnvironment()->addNew(building, bulkAdd);
	} else {
		Game::getLog()->Write(0, "Building loading not possible");
	}
}

void SimulationObjectManager::addResource(ResourceEntity* resource, bool bulkAdd) const {
	if (resource) {
		resources->push_back(resource);
		Game::getEnvironment()->addNew(resource, bulkAdd);
	} else {
		Game::getLog()->Write(0, "Resource adding not possible");
	}
}

void SimulationObjectManager::findToDisposeUnits() {
	if (StateManager::isUnitToDispose()) {
		units->erase( //TODO bug?
		             std::remove_if(
		                            units->begin(), units->end(),
		                            [this](Unit* unit) {
			                            if (unit->isToDispose()) {
				                            unitsToDispose.push_back(unit);
				                            return true;
			                            }
			                            unit->clean();
			                            return false;
		                            }
		                           ), units->end());
	}
}

void SimulationObjectManager::findToDisposeBuildings() {
	if (StateManager::isBuildingToDispose()) {
		buildings->erase(
		                 std::remove_if(
		                                buildings->begin(), buildings->end(),
		                                [this](Building* building) {
			                                if (building->isToDispose()) {
				                                buildingsToDispose.push_back(building);
				                                return true;
			                                }
			                                return false;
		                                }
		                               ), buildings->end());
	}
}

void SimulationObjectManager::findToDisposeResources() {
	if (StateManager::isResourceToDispose()) {
		resources->erase(
		                 std::remove_if(
		                                resources->begin(), resources->end(),
		                                [this](ResourceEntity* resource) {
			                                if (resource->isToDispose()) {
				                                resourcesToDispose.push_back(resource);
				                                return true;
			                                }
			                                return false;
		                                }
		                               ), resources->end());
	}
}

void SimulationObjectManager::refreshResBonuses() {
	//TODO perf ugly, refresh ca³oœci
	bool resourceBuildingChanged[MAX_PLAYERS][RESOURCES_SIZE] =
	{
		{false, false, false, false},
		{false, false, false, false}
	};
	for (const auto building : buildingsToDispose) {
		auto& vals = resourceBuildingChanged[building->getPlayer()];
		for (const char resId : building->getDbBuilding()->resourceTypes) {
			vals[resId] = true;
		}
	}
	for (char p = 0; p < MAX_PLAYERS; ++p) {
		const auto& perPlayer = resourceBuildingChanged[p];
		for (char r = 0; r < RESOURCES_SIZE; ++r) {
			if (perPlayer[r]) {
				Game::getEnvironment()->reAddBonuses(buildings, p, r);
			}
		}
	}
}

void SimulationObjectManager::dispose() {
	Game::getEnvironment()->removeFromGrids(unitsToDispose);
	Game::getEnvironment()->removeFromGrids(buildingsToDispose, resourcesToDispose);

	refreshResBonuses();

	clear_vector(unitsToDispose);
	clear_vector(buildingsToDispose);
	clear_vector(resourcesToDispose);
}
