#include "ObjectList.h"



ObjectList::ObjectList() {
	units = new std::vector<Unit*>();
	buildings = new std::vector<Building*>();
	entities = new std::vector<Entity*>();
}


ObjectList::~ObjectList() {
	delete units;
	delete buildings;
	delete entities;
}

void ObjectList::add(Unit * unit) {
	units->push_back(unit);
	entities->push_back(unit);
}

void ObjectList::add(Entity * entity) {
	entities->push_back(entity);
}

void ObjectList::add(Building* building) {
	buildings->push_back(building);
	entities->push_back(building);
}

void ObjectList::addAll(std::vector<Unit*> *_units) {

}

void ObjectList::addAll(std::vector<Building*> *_buildings) {

}

void ObjectList::addAll(std::vector<Entity*> *_entities) {

}

std::vector<Unit*>* ObjectList::getUnits() {
	return units;
}

std::vector<Building*> *ObjectList::getBuildings() {
	return buildings;
}

std::vector<Entity*>* ObjectList::getEntities() {
	return entities;
}
