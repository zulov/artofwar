#include "CreationCommandList.h"
#include "simulation/env/Enviroment.h"


CreationCommandList::CreationCommandList(SimulationObjectManager* _simulationObjectManager) {
	simulationObjectManager = _simulationObjectManager;
}

CreationCommandList::~CreationCommandList() {
}

bool CreationCommandList::addUnits(int _number, int id, Vector3* _position, SpacingType _spacingType, int _player) {
	add(new CreationCommand(UNIT, _number, id, _position, _spacingType, _player));
	return true;
}

bool CreationCommandList::addBuilding(int id, Vector3* _position, int _player) {
	Resources* resources = Game::get()->getPlayersManager()->getActivePlayer()->getResources();
	std::vector<db_cost*>* costs = Game::get()->getDatabaseCache()->getCostForBuilding(id);
	Enviroment* env = Game::get()->getEnviroment();
	db_building* db_building = Game::get()->getDatabaseCache()->getBuilding(id);

	if (env->validateStatic(db_building->size, _position) && resources->reduce(costs)) {
		_position = env->getValidPosition(db_building->size, _position);

		add(new CreationCommand(BUILDING, 0, id, _position, CONSTANT, _player));
		return true;
	}
	return false;
}

bool CreationCommandList::addResource(int id, Vector3* _position) {
	Enviroment* env = Game::get()->getEnviroment();
	db_resource* db_resource = Game::get()->getDatabaseCache()->getResource(id);

	if (env->validateStatic(db_resource->size, _position)) {
		_position = env->getValidPosition(db_resource->size, _position);

		add(new CreationCommand(RESOURCE, 0, id, _position, CONSTANT, -1));
		return true;
	}
	return false;
}

void CreationCommandList::setParemeters(AbstractCommand* command) {
	CreationCommand* m = static_cast<CreationCommand *>(command);
	m->setSimulationObjectManager(simulationObjectManager);
}
