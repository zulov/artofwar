#pragma once

#include "SimulationInfo.h"
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
	void addResource(int id, Urho3D::Vector2& center, const Urho3D::IntVector2& _bucketCords, int level) const;

	void prepareToDispose() const;
	void updateInfo(SimulationInfo* simulationInfo) const;
	void dispose();

	void load(dbload_unit* unit);
	void load(dbload_building* building);
	void load(dbload_resource_entities* resource) const;

private:

	void updateUnits(std::vector<Unit*>* temp) const;
	void updateBuilding(std::vector<Building*>* temp) const;
	void updateResource(std::vector<ResourceEntity*>* temp) const;

	bool shouldDelete(Physical* physical);

	void add(Unit* unit) const { units->push_back(unit); }
	void add(Building* building) const { buildings->push_back(building); }
	void add(ResourceEntity* resourceEntity) const { resources->push_back(resourceEntity); }

	UnitFactory unitFactory;
	BuildingFactory buildingFactory;
	ResourceFactory resourceFactory;

	SimulationInfo simulationInfo;

	std::vector<Unit*>* units;
	std::vector<Building*>* buildings;
	std::vector<ResourceEntity*>* resources;

	std::vector<Physical*> toDisposePhysical;

	std::function<bool(Physical*)> physicalShouldDelete = std::bind(&SimulationObjectManager::shouldDelete, this,
	                                                                std::placeholders::_1);
	template <class T>
	void prepareToDispose(std::vector<T*> * objects) const;
};
