#pragma once

#include <vector>
#include "Unit.h"
#include "Entity.h"
#include "Building.h"
#include "BuildingFactory.h"
#include "UnitFactory.h"

class ObjectManager {
public:
	ObjectManager(ResourceCache* _cache, SharedPtr<Scene> _scene);
	~ObjectManager();
	void add(Unit* unit);
	void add(Entity* entity);
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
	std::vector<Entity*> *entities;

	UnitFactory * unitFactory;
	BuildingFactory * buildingFactory;
	SharedPtr<Scene> scene;

};

