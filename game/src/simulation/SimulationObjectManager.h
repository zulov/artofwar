#pragma once

#include <functional>
#include <vector>
#include "objects/building/BuildingFactory.h"
#include "objects/resource/ResourceFactory.h"
#include "objects/unit/UnitFactory.h"


class Physical;

class SimulationObjectManager {
public:
	SimulationObjectManager();
	~SimulationObjectManager();
	void clearNodesWithoutDelete();

	std::vector<Unit*>* getUnits() const { return units; }
	std::vector<Building*>* getBuildings() const { return buildings; }
	std::vector<ResourceEntity*>* getResources() const { return resources; }

	void addUnits(unsigned number, int id, Urho3D::Vector2& center, char level, char player);
	void addBuilding(int id, const Urho3D::IntVector2& _bucketCords, char level, char player) const;
	void addResource(int id, const Urho3D::IntVector2& _bucketCords, char level) const;

	void findToDispose();

	void dispose();

	void load(dbload_unit* unit);
	void load(dbload_building* building);
	void load(dbload_resource_entities* resource) const;

private:
	void addUnits(std::vector<Unit*>& temp) const;
	void addBuilding(Building* building) const;
	void addResource(ResourceEntity* resource, bool bulkAdd) const;

	void findToDisposeUnits();
	void findToDisposeBuildings();
	void findToDisposeResources();
	void refreshResBonuses();

	UnitFactory unitFactory;
	BuildingFactory buildingFactory;
	ResourceFactory resourceFactory;

	std::vector<Unit*>* units;
	std::vector<Building*>* buildings;
	std::vector<ResourceEntity*>* resources;

	std::vector<Unit*> unitsToDispose;
	std::vector<Building*> buildingsToDispose;
	std::vector<ResourceEntity*> resourcesToDispose;

};
