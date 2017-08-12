#pragma once

#include <vector>
#include <Urho3D/Graphics/StaticModelGroup.h>
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

	vector<Unit*>* getUnits();
	vector<Building*>* getBuildings();
	vector<Entity*>* getEntities();
	vector<ResourceEntity*>* getResources();

	void addUnits(unsigned number, int id, Vector3* center, SpacingType spacingType, int player);
	void addBuildings(unsigned number, int id, Vector3* center, SpacingType spacingType, int player);
	void addResources(unsigned number, int id, Vector3* center, SpacingType spacingType);
private:
	vector<Unit*>* units;
	vector<Building*>* buildings;
	vector<ResourceEntity*>* resources;

	void add(Unit* unit);
	void add(Entity* entity) override;
	void add(Building* building);
	void add(ResourceEntity* resourceEntity);

	void addAll(vector<Unit*>* _units);
	void addAll(vector<Building*>* _buildings);
	void addAll(vector<Entity*>* _entities);
	void addAll(vector<ResourceEntity*>* _resources);

	UnitFactory* unitFactory;
	BuildingFactory* buildingFactory;
	ResourceFactory* resourceFactory;
};
