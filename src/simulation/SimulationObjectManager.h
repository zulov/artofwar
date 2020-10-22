#pragma once

#include "SimulationInfo.h"
#include "objects/building/BuildingFactory.h"
#include "objects/resource/ResourceFactory.h"
#include "objects/unit/UnitFactory.h"
#include <functional>
#include <vector>


class Physical;
class SimulationInfo;

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

	void prepareToDispose() const;
	void updateInfo(SimulationInfo* simulationInfo);
	void dispose();

	void load(dbload_unit* unit);
	void load(dbload_building* building);
	void load(dbload_resource_entities* resource);
	std::vector<Physical*>& getToDispose();

private:

	void updateUnits(std::vector<Unit*>& temp);
	void updateBuilding(Building* building);
	void updateResource(ResourceEntity* resource);

	UnitFactory unitFactory;
	BuildingFactory buildingFactory;
	ResourceFactory resourceFactory;

	SimulationInfo simulationInfo;

	std::vector<Unit*>* units;
	std::vector<Building*>* buildings;
	std::vector<ResourceEntity*>* resources;

	std::vector<Physical*> toDisposePhysical;

	template <class T>
	void prepareToDispose(std::vector<T*>* objects) const;
};
