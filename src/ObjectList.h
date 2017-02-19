#pragma once

#include <vector>
#include "Unit.h"
#include "Entity.h"
#include "Building.h"

class ObjectList {
public:
	ObjectList();
	~ObjectList();
	void add(Unit* unit);
	void add(Entity* entity);
	void add(Building* building);

	void addAll(std::vector<Unit*> *_units);
	void addAll(std::vector<Building*> *_buildings);
	void addAll(std::vector<Entity*> *_entities);

	std::vector<Unit*> *getUnits();
	std::vector<Building*> *getBuildings();
	std::vector<Entity*>* getEntities();
private:

	std::vector<Unit*> *units;
	std::vector<Building*> *buildings;
	std::vector<Entity*> *entities;

};

