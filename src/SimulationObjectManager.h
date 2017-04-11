#pragma once

#include <vector>
#include "Unit.h"
#include "Entity.h"
#include "Building.h"
#include "BuildingFactory.h"
#include "UnitFactory.h"
#include "AbstractObjectManager.h"
#include "SceneObjectManager.h"

class SimulationObjectManager :public AbstractObjectManager {
public:
	SimulationObjectManager();
	~SimulationObjectManager();
	void add(Unit* unit);
	void add(Entity* entity) override;
	void add(Building* building);

	void addAll(std::vector<Unit*> *_units);
	void addAll(std::vector<Building*> *_buildings);
	void addAll(std::vector<Entity*> *_entities);

	std::vector<Unit*> *getUnits();
	std::vector<Building*> *getBuildings();
	std::vector<Entity*>* getEntities();

	std::vector<Unit*>* createUnits();
private:
	std::vector<Unit*> *units;
	std::vector<Building*> *buildings;

	UnitFactory * unitFactory;
	BuildingFactory * buildingFactory;

};

