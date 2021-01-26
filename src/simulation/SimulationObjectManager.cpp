#include "SimulationObjectManager.h"

#include <Urho3D/IO/Log.h>

#include "Game.h"
#include "ObjectsInfo.h"
#include "objects/building/Building.h"
#include "objects/resource/ResourceEntity.h"
#include "objects/unit/Unit.h"
#include "objects/unit/state/StateManager.h"
#include "player/Player.h"
#include "player/PlayersManager.h"
#include "scene/load/dbload_container.h"
#include "simulation/env/Environment.h"


SimulationObjectManager::SimulationObjectManager() {
	units = new std::vector<Unit*>();
	buildings = new std::vector<Building*>();
	resources = new std::vector<ResourceEntity*>();

	units->reserve(4069);
	buildings->reserve(128);
	resources->reserve(20 * 1024);
	simulationInfo = new ObjectsInfo();
}


SimulationObjectManager::~SimulationObjectManager() {
	clear_and_delete_vector(units);
	clear_and_delete_vector(buildings);
	clear_and_delete_vector(resources);

	delete simulationInfo;

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
	addBuilding(buildingFactory.create(id, _bucketCords, level, player));
}

void SimulationObjectManager::addResource(int id, const Urho3D::IntVector2& _bucketCords, char level) const {
	addResource(resourceFactory.create(id, _bucketCords, level));
}

void SimulationObjectManager::findToDispose() {
	findToDisposeUnits(); //TODO perf jezeli cos zmieni³o stan do dispose
	findToDisposeBuildings();
	findToDisposeResources();
}

void SimulationObjectManager::load(dbload_unit* unit) {
	addUnits(unitFactory.load(unit));
}

void SimulationObjectManager::load(dbload_building* building) {
	addBuilding(buildingFactory.load(building));
}

void SimulationObjectManager::load(dbload_resource_entities* resource) const {
	addResource(resourceFactory.load(resource));
}

void SimulationObjectManager::addUnits(std::vector<Unit*>& temp) const {
	if (!temp.empty()) {
		units->insert(units->end(), temp.begin(), temp.end());
		for (auto value : temp) {
			Game::getPlayersMan()->getPlayer(value->getPlayer())->add(value);
		}
		Game::getEnvironment()->addNew(temp);
		simulationInfo->setAmountUnitChanged();
	}
}


void SimulationObjectManager::addBuilding(Building* building) const {
	if (building) {
		buildings->push_back(building);

		Game::getPlayersMan()->getPlayer(building->getPlayer())->add(building);
		Game::getEnvironment()->addNew(building);
		simulationInfo->setAmountBuildingChanged();

		Game::getEnvironment()->updateAll(buildings);
	} else {
		Game::getLog()->Write(0, "Building loading not possible");
	}

}

void SimulationObjectManager::addResource(ResourceEntity* resource) const {
	if (resource) {
		resources->push_back(resource);
		Game::getEnvironment()->addNew(resource);
		simulationInfo->setAmountResourceChanged();
	} else {
		Game::getLog()->Write(0, "Resource adding not possible");
	}
}

void SimulationObjectManager::findToDisposeUnits() {
	units->erase(
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

	if (!unitsToDispose.empty()) {
		simulationInfo->setUnitDied();
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

		if (!buildingsToDispose.empty()) {
			simulationInfo->setBuildingDied();
		}
		StateManager::setBuildingToDispose(false);
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

		if (!resourcesToDispose.empty()) {
			simulationInfo->setResourceDied();
		}
		StateManager::setResourceToDispose(false);
	}
}

void SimulationObjectManager::dispose() {
	Game::getEnvironment()->removeFromGrids(unitsToDispose);
	Game::getEnvironment()->removeFromGrids(buildingsToDispose, resourcesToDispose);

	clear_vector(unitsToDispose);
	clear_vector(buildingsToDispose);
	clear_vector(resourcesToDispose);

	simulationInfo->reset();
}
