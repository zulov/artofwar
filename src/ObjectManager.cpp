#include "ObjectManager.h"


ObjectManager::ObjectManager() {
	units = new std::vector<Unit*>();
	buildings = new std::vector<Building*>();
	entities = new std::vector<Entity*>();
	unitFactory = new UnitFactory();
	buildingFactory = new BuildingFactory();
}


ObjectManager::~ObjectManager() {
	delete units;
	delete buildings;
	delete entities;

	delete unitFactory;
	delete buildingFactory;
}

void ObjectManager::add(Unit* unit) {
	units->push_back(unit);
	entities->push_back(unit);
}

void ObjectManager::add(Entity* entity) {
	entities->push_back(entity);
	createLink(entity->getNode(), entity);
}

void ObjectManager::add(Building* building) {
	buildings->push_back(building);
	entities->push_back(building);
}

void ObjectManager::addAll(std::vector<Unit*>* _units) {

}

void ObjectManager::addAll(std::vector<Building*>* _buildings) {

}

void ObjectManager::addAll(std::vector<Entity*>* _entities) {

}

std::vector<Unit*>* ObjectManager::getUnits() {
	return units;
}

std::vector<Building*>* ObjectManager::getBuildings() {
	return buildings;
}

std::vector<Entity*>* ObjectManager::getEntities() {
	return entities;
}

std::vector<Unit*>* ObjectManager::createUnits() {
	return unitFactory->createUnits();
}


void ObjectManager::createLink(Node* node, Entity* entity) {
	LinkComponent* lc = node->CreateComponent<LinkComponent>();

	lc->bound(node, entity);
}