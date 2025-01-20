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
#include "math/VectorUtils.h"


SimulationObjectManager::SimulationObjectManager(unsigned currentResourceUid): resourceFactory(currentResourceUid) {
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

	//removeFromGrids();
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

ResourceEntity* SimulationObjectManager::addResource(int id, const Urho3D::IntVector2& _bucketCords) {
	if (const auto res = resourceFactory.create(id, _bucketCords)) {
		addResource(res, false);
		return res;
	}
	return nullptr;
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
	Game::getEnvironment()->refreshAllStatic(resources, buildings);
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

void SimulationObjectManager::cleanAndDisposeUnits() {
	if (StateManager::isSthToDispose()) {
		for (const auto unit : *units) {
			unit->clean(); //TODO bug? to powinno być niepotrzebne
		}
	}
	cleanAndDispose(units, StateManager::isUnitToDispose());
}

void SimulationObjectManager::cleanAndDisposeBuildings() {
	cleanAndDispose(buildings, StateManager::isBuildingToDispose());
}

void SimulationObjectManager::cleanAndDisposeResources() {
	cleanAndDispose(resources, StateManager::isResourceToDispose());
}

void SimulationObjectManager::refreshResBonuses() {
	auto isResourceBuilding = [](const Building* building){
		return building->getDb()->typeResourceAny;
	};

	if (!std::ranges::any_of(StateManager::getDeadBuildings(), isResourceBuilding)) { return; }

	std::vector<Building*> resBuilding;
	std::ranges::copy_if(*buildings, std::back_inserter(resBuilding), isResourceBuilding);

	Game::getEnvironment()->reAddBonuses(resBuilding, resources);
}

void SimulationObjectManager::dispose() {
	cleanAndDisposeUnits();
	cleanAndDisposeBuildings();
	cleanAndDisposeResources();
}

void SimulationObjectManager::removeFromGrids() {
	Game::getEnvironment()->removeFromGrids(StateManager::getDeadUnits());
	Game::getEnvironment()->removeFromGrids(StateManager::getDeadBuildings(), StateManager::getDeadResources());

	refreshResBonuses();
}
