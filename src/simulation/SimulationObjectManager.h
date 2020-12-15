#pragma once

#include "objects/building/BuildingFactory.h"
#include "objects/resource/ResourceFactory.h"
#include "objects/unit/UnitFactory.h"
#include <functional>
#include <vector>


class Physical;
class ObjectsInfo;

class SimulationObjectManager {
public:
	SimulationObjectManager();
	~SimulationObjectManager();

	std::vector<Unit*>* getUnits() const { return units; }
	std::vector<Building*>* getBuildings() const { return buildings; }
	std::vector<ResourceEntity*>* getResources() const { return resources; }

	void addUnits(unsigned number, int id, Urho3D::Vector2& center, int player, int level);
	void addBuilding(int id, Urho3D::Vector2& center, int player, const Urho3D::IntVector2& _bucketCords, int level);
	void addResource(int id, Urho3D::Vector2& center, const Urho3D::IntVector2& _bucketCords, int level);

	void findToDispose();

	void dispose();

	void load(dbload_unit* unit);
	void load(dbload_building* building);
	void load(dbload_resource_entities* resource) const;
	ObjectsInfo* getInfo() const { return simulationInfo; }

private:
	void addUnits(std::vector<Unit*>& temp) const;
	void addBuilding(Building* building) const;
	void addResource(ResourceEntity* resource) const;

	void findToDisposeUnits();
	void findToDisposeBuildings();
	void findToDisposeResources();

	UnitFactory unitFactory;
	BuildingFactory buildingFactory;
	ResourceFactory resourceFactory;

	ObjectsInfo* simulationInfo;

	std::vector<Unit*>* units;
	std::vector<Building*>* buildings;
	std::vector<ResourceEntity*>* resources;

	std::vector<Unit*> unitsToDispose;
	std::vector<Building*> buildingsToDispose;
	std::vector<ResourceEntity*> resourcesToDispose;

};
