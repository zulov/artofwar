#pragma once

#include <vector>
#include "Unit.h"
#include "Entity.h"
#include "Building.h"
#include "BuildingFactory.h"
#include "UnitFactory.h"
#include "AbstractObjectManager.h"
#include "ResourceFactory.h"

class SimulationObjectManager :public AbstractObjectManager
{
public:
	SimulationObjectManager();
	~SimulationObjectManager();

	std::vector<Unit*>* getUnits();
	std::vector<Building*>* getBuildings();
	std::vector<Entity*>* getEntities();
	std::vector<ResourceEntity*>* getResources();

	void addUnits(unsigned number, UnitType unitType, Vector3* center, SpacingType spacingType, int player);
	void addBuildings(unsigned number, BuildingType buildingType, Vector3* center, SpacingType spacingType, int player);
	void addResources(unsigned number, ResourceType resourceType, Vector3* center, SpacingType spacingType);
private:
	std::vector<Unit*>* units;
	std::vector<Building*>* buildings;
	std::vector<ResourceEntity*>* resources;

	void add(Unit* unit);
	void add(Entity* entity) override;
	void add(Building* building);
	void add(ResourceEntity* resourceEntity);

	void addAll(std::vector<Unit*>* _units);
	void addAll(std::vector<Building*>* _buildings);
	void addAll(std::vector<Entity*>* _entities);
	void addAll(std::vector<ResourceEntity*>* _resources);

	UnitFactory* unitFactory;
	BuildingFactory* buildingFactory;
	ResourceFactory* resourceFactory;
};
