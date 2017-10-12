#pragma once

#include <vector>
#include <Urho3D/Graphics/StaticModelGroup.h>
#include "objects/unit/Unit.h"
#include "objects/Entity.h"
#include "objects/building/Building.h"
#include "objects/building/BuildingFactory.h"
#include "objects/unit/UnitFactory.h"
#include "objects/resource/ResourceFactory.h"
#include "SimulationInfo.h"

class SimulationObjectManager
{
public:
	SimulationObjectManager();
	~SimulationObjectManager();

	vector<Unit*>* getUnits();
	vector<Building*>* getBuildings();
	vector<ResourceEntity*>* getResources();

	vector<Unit*>* getUnitsToAdd();
	vector<Building*>* getBuildingsToAdd();
	vector<ResourceEntity*>* getResourcesToAdd();

	void addUnits(unsigned number, int id, Vector3* center, SpacingType spacingType, int player);
	void addBuildings(int id, Vector3* center, SpacingType spacingType, int player);
	void addResources(unsigned number, int id, Vector3* center, SpacingType spacingType);
	void clearUnitsToAdd();
	void clearBuildingsToAdd();
	void clearResourcesToAdd();
	void clean();
	void updateInfo(SimulationInfo* simulationInfo);
	void dispose();
private:
	bool shouldDelete(Physical* physical);

	void add(Unit* unit);
	void add(Building* building);
	void add(ResourceEntity* resourceEntity);

	void addAll(vector<Unit*>* _units);
	void addAll(vector<Building*>* _buildings);
	void addAll(vector<ResourceEntity*>* _resources);

	UnitFactory* unitFactory;
	BuildingFactory* buildingFactory;
	ResourceFactory* resourceFactory;

	SimulationInfo* simulationInfo;

	vector<Unit*>* units;
	vector<Building*>* buildings;
	vector<ResourceEntity*>* resources;
	vector<Physical*>* toDispose;

	vector<Unit*>* unitsToAdd;
	vector<Building*>* buildingsToAdd;
	vector<ResourceEntity*>* resourcesToAdd;

	vector<Unit*>* unitsTemp;
	vector<Building*>* buildingsTemp;
	vector<ResourceEntity*>* resourcesTemp;
};
