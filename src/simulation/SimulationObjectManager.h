#pragma once

#include "objects/building/BuildingFactory.h"
#include "objects/resource/ResourceFactory.h"
#include "objects/unit/UnitFactory.h"
#include <functional>
#include <vector>


class Physical;
class SimulationInfo;

class SimulationObjectManager
{
public:
	SimulationObjectManager();
	~SimulationObjectManager();

	std::vector<Unit*>* getUnits() const { return units; }
	std::vector<Building*>* getBuildings() const { return buildings; }
	std::vector<ResourceEntity*>* getResources() const { return resources; }

	void addUnits(unsigned number, int id, Urho3D::Vector2& center, int player, int level);
	void addBuilding(int id, Urho3D::Vector2& center, int player, const Urho3D::IntVector2& _bucketCords, int level);
	void addResource(int id, Urho3D::Vector2& center, const Urho3D::IntVector2& _bucketCords, int level);

	void prepareToDispose() const;
	void updateInfo(SimulationInfo* simulationInfo) const;
	void dispose();

	void load(dbload_unit* unit);
	void load(dbload_building* building);
	void load(dbload_resource_entities* resource);

private:

	void updateUnits(std::vector<Unit*>* temp);
	void updateBuilding(std::vector<Building*>* temp);
	void updateResource(std::vector<ResourceEntity*>* temp);

	bool shouldDelete(Physical* physical);

	void add(Unit* unit) const { units->push_back(unit); }
	void add(Building* building) const { buildings->push_back(building); }
	void add(ResourceEntity* resourceEntity) const { resources->push_back(resourceEntity); }

	void addAll(std::vector<Unit*>* _units) const;
	void addAll(std::vector<Building*>* _buildings) const;
	void addAll(std::vector<ResourceEntity*>* _resources) const;

	UnitFactory unitFactory;
	BuildingFactory buildingFactory;
	ResourceFactory resourceFactory;

	SimulationInfo* simulationInfo;

	std::vector<Unit*>* units;
	std::vector<Building*>* buildings;
	std::vector<ResourceEntity*>* resources;

	//std::vector<Unit*> toDisposeUnit;
	//std::vector<Building*> toDisposeBuilding;
	//std::vector<ResourceEntity*> toDisposeResource;
	std::vector<Physical*> toDisposePhysical;

	std::vector<Building*>* buildingsToAdd;
	std::vector<ResourceEntity*>* resourcesToAdd;

	std::function<bool(Physical*)> physicalShouldDelete = std::bind(&SimulationObjectManager::shouldDelete, this,
	                                                                std::placeholders::_1);
	template <class T>
	void prepareToDispose(std::vector<T*> * objects) const;
};
