#include "SimulationObjectManager.h"
#include "Game.h"
#include "SimulationInfo.h"
#include "objects/building/Building.h"
#include "objects/resource/ResourceEntity.h"
#include "objects/unit/Unit.h"
#include "player/Player.h"
#include "player/PlayersManager.h"
#include "simulation/env/Environment.h"


SimulationObjectManager::SimulationObjectManager() {
	units = new std::vector<Unit*>();
	buildings = new std::vector<Building*>();
	resources = new std::vector<ResourceEntity*>();

	units->reserve(8192);
	buildings->reserve(128);
	resources->reserve(1024);
	simulationInfo = new SimulationInfo();
}


SimulationObjectManager::~SimulationObjectManager() {
	clear_and_delete_vector(units);
	clear_and_delete_vector(buildings);
	clear_and_delete_vector(resources);
	
	delete simulationInfo;

	dispose();
}

void SimulationObjectManager::addUnits(unsigned int number, int id, Urho3D::Vector2& center,
                                       int player, int level) {
	addUnits(unitFactory.create(number, id, center, player, level));
}

void SimulationObjectManager::addBuilding(int id, Urho3D::Vector2& center, int player,
                                          const Urho3D::IntVector2& _bucketCords, int level) {
	addBuilding(buildingFactory.create(id, center, player, _bucketCords, level));
}


void SimulationObjectManager::addResource(int id, Urho3D::Vector2& center, const Urho3D::IntVector2& _bucketCords,
                                          int level) {
	addResource(resourceFactory.create(id, center, _bucketCords, level));
}

void SimulationObjectManager::findToDispose() {
	findToDisposeUnits();
	findToDisposeBuildings();
	findToDisposeResources();
}

void SimulationObjectManager::load(dbload_unit* unit) {
	addUnits(unitFactory.load(unit));
}

void SimulationObjectManager::load(dbload_building* building) {
	addBuilding(buildingFactory.load(building));
}

void SimulationObjectManager::load(dbload_resource_entities* resource) {
	addResource(resourceFactory.load(resource));
}

void SimulationObjectManager::addUnits(std::vector<Unit*>& temp) {
	if (!temp.empty()) {
		units->insert(units->end(), temp.begin(), temp.end());
		for (auto value : temp) {
			Game::getPlayersMan()->getPlayer(value->getPlayer())->add(value);
		}
		Game::getEnvironment()->addNew(temp);
		simulationInfo->setAmountUnitChanged();
	}
}

void SimulationObjectManager::addBuilding(Building* building) {
	buildings->push_back(building);

	Game::getPlayersMan()->getPlayer(building->getPlayer())->add(building);
	Game::getEnvironment()->addNew(building);
	simulationInfo->setAmountBuildingChanged();

	Game::getEnvironment()->updateAll(buildings);
}

void SimulationObjectManager::addResource(ResourceEntity* resource) {
	resources->push_back(resource);
	Game::getEnvironment()->addNew(resource);
	simulationInfo->setAmountResourceChanged();
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

	if (!unitsToDispose.empty()) {
		simulationInfo->setBuildingDied();
	}
}

void SimulationObjectManager::findToDisposeResources() {
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

	if (!unitsToDispose.empty()) {
		simulationInfo->setResourceDied();
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
