#include "SimulationObjectManager.h"
#include "Game.h"
#include "SimulationInfo.h"
#include "objects/building/Building.h"
#include "objects/resource/ResourceEntity.h"
#include "objects/unit/Unit.h"
#include <algorithm>
#include <simulation/env/Environment.h>
#include "player/PlayersManager.h"
#include "player/Player.h"


SimulationObjectManager::SimulationObjectManager() {
	units = new std::vector<Unit*>();
	buildings = new std::vector<Building*>();
	resources = new std::vector<ResourceEntity*>();

	units->reserve(10000);
	buildings->reserve(100);
	resources->reserve(1000);

	toDisposePhysical.reserve(1000);
}


SimulationObjectManager::~SimulationObjectManager() {
	clear_and_delete_vector(units);
	clear_and_delete_vector(buildings);
	clear_and_delete_vector(resources);
}

void SimulationObjectManager::addUnits(unsigned int number, int id, Urho3D::Vector2& center,
                                       int player, int level) {
	updateUnits(unitFactory.create(number, id, center, player, level));
}

void SimulationObjectManager::addBuilding(int id, Urho3D::Vector2& center, int player,
                                          const Urho3D::IntVector2& _bucketCords, int level) {
	updateBuilding(buildingFactory.create(id, center, player, _bucketCords, level));
}


void SimulationObjectManager::addResource(int id, Urho3D::Vector2& center, const Urho3D::IntVector2& _bucketCords,
                                          int level) {
	updateResource(resourceFactory.create(id, center, _bucketCords, level));
}

template <class T>
void SimulationObjectManager::prepareToDispose(std::vector<T*>* objects) const {
	objects->erase( //TODO performance iterowac tylko jezeli ktos umarl - przemyslec to
		std::remove_if(
			objects->begin(), objects->end(),
			[this](Physical* physical) {
				if (physical->isToDispose()) {
					toDisposePhysical.push_back(physical);
					simulationInfo.setSthDied(physical->getType());
					return true;
				}
				physical->clean();
				return false;
			}
			//physicalShouldDelete
		),
		objects->end());
}

void SimulationObjectManager::prepareToDispose() const {
	prepareToDispose(units);
	prepareToDispose(buildings);
	prepareToDispose(resources);
}

void SimulationObjectManager::load(dbload_unit* unit) {
	updateUnits(unitFactory.load(unit));
}

void SimulationObjectManager::load(dbload_building* building) {
	updateBuilding(buildingFactory.load(building));
}

void SimulationObjectManager::load(dbload_resource_entities* resource) {
	updateResource(resourceFactory.load(resource));
}

std::vector<Physical*>& SimulationObjectManager::getToDispose() {
	return toDisposePhysical;
}

void SimulationObjectManager::updateUnits(std::vector<Unit*>& temp) {
	if (!temp.empty()) {
		units->insert(units->end(), temp.begin(), temp.end());
		for (auto value : temp) {
			Game::getPlayersMan()->getPlayer(value->getPlayer())->add(value);
		}
		simulationInfo.setAmountUnitChanged();
	}
}

void SimulationObjectManager::updateBuilding(Building* building) {
	buildings->push_back(building);

	Game::getPlayersMan()->getPlayer(building->getPlayer())->add(building);
	Game::getEnvironment()->update(building);
	simulationInfo.setAmountBuildingChanged();

	Game::getEnvironment()->updateAll(buildings);
}

void SimulationObjectManager::updateResource(ResourceEntity* resource) {
	resources->push_back(resource);
	Game::getEnvironment()->update(resource);
	simulationInfo.setAmountResourceChanged();
}

void SimulationObjectManager::updateInfo(SimulationInfo* simulationInfo) {
	auto a = this->simulationInfo;
	simulationInfo->set(a);
	this->simulationInfo.reset();
}

void SimulationObjectManager::dispose() {
	if (!toDisposePhysical.empty()) {
		clear_vector(toDisposePhysical);
	}
	simulationInfo.reset();
}
