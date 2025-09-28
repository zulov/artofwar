#pragma once

#include <functional>
#include <vector>
#include "objects/building/BuildingFactory.h"
#include "objects/resource/ResourceFactory.h"
#include "objects/unit/UnitFactory.h"


class Physical;

class SimulationObjectManager {
public:
	SimulationObjectManager(unsigned currentResourceUid);
	~SimulationObjectManager();
	void clearNodesWithoutDelete();

	std::vector<Unit*>* getUnits() const { return units; }
	std::vector<Building*>* getBuildings() const { return buildings; }
	std::vector<ResourceEntity*>* getResources() const { return resources; }

	void addUnits(unsigned number, int id, Urho3D::Vector2& center, char level, char player);
	void addBuilding(int id, const Urho3D::IntVector2& _bucketCords, char level, char player) const;
	ResourceEntity* addResource(int id, const Urho3D::IntVector2& _bucketCords);

	void dispose();

	void removeFromGrids();

	void load(dbload_unit* unit);
	void load(dbload_building* building) const;
	void load(dbload_resource* resource) const;
	void refreshAllStatic();

private:
	void addUnits(std::vector<Unit*>& temp) const;
	void addBuilding(Building* building, bool bulkAdd) const;
	void addResource(ResourceEntity* resource, bool bulkAdd) const;

	void cleanAndDisposeUnits();
	void cleanAndDisposeBuildings();
	void cleanAndDisposeResources();
	void refreshResBonuses();

	UnitFactory unitFactory;
	BuildingFactory buildingFactory;
	ResourceFactory resourceFactory;

	std::vector<Unit*>* units;
	std::vector<Building*>* buildings;
	std::vector<ResourceEntity*>* resources;
};
