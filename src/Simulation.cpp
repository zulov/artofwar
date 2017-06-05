#include "Simulation.h"


Simulation::Simulation(EnviromentStrategy* _enviromentStrategy, SimulationCommandList* _simCommandList, SimulationObjectManager* _simObjectManager) {
	envStrategy = _enviromentStrategy;
	forceStrategy = new ForceStrategy();
	simObjectManager = _simObjectManager;
	srand(time(NULL));
	animate = true;
	simCommandList = _simCommandList;
	aimContainer = new AimContainer();
	actionCommandList = new ActionCommandList(aimContainer);
	Game::get()->setActionCommmandList(actionCommandList);
	createUnits();
}

void Simulation::action(float timeStep) {
	timeStep = 0.5;
	for (unsigned i = 0; i < units->size(); ++i) {
		Unit* unit = (*units)[i];
		std::vector<Entity*>* neighbours = envStrategy->getNeighboursFromTeam(unit, 6, unit->getTeam(), NOT_EQUAL);
		std::vector<Entity*>* enemies = new std::vector<Entity*>();
		enemies->reserve(neighbours->size());

		for (int j = 0; j < neighbours->size(); ++j) {
			Entity* entity = (*neighbours)[j];
			if (unit->getTeam() != entity->getTeam()) {
				enemies->push_back(entity);
			}
		}
		unit->attack(enemies);
		delete enemies;
		delete neighbours;
	}
}

void Simulation::createUnits() {
	simCommandList->add(new SimulationCommand(UNITS_NUMBER, UnitType::WARRIOR, new Vector3(0, 0, 0), SpacingType::CONSTANT, 0));
	simCommandList->add(new SimulationCommand(UNITS_NUMBER, UnitType::ARCHER, new Vector3(-50, 0, -50), SpacingType::CONSTANT, 1));
}

void Simulation::move(float timeStep) {
	calculateForces();
	moveUnits(timeStep);
}

void Simulation::update(Input* input, float timeStep) {
	if (input->GetKeyPress(KEY_SPACE)) {
		animate = !animate;
	}

	if (animate) {
		simCommandList->execute();
		actionCommandList->execute();

		units = simObjectManager->getUnits();
		buildings = simObjectManager->getBuildings();

		envStrategy->update(units);
		envStrategy->update(buildings);

		move(timeStep);
		
		action(timeStep);
		aimContainer->clean();
	}
}

void Simulation::moveUnits(float timeStep) {
	timeStep = 0.05;
	for (unsigned i = 0; i < units->size(); ++i) {
		Unit* unit = (*units)[i];
		unit->applyForce(timeStep);
		unit->move(timeStep);
	}
}

void Simulation::calculateForces() {
	for (unsigned i = 0; i < units->size(); ++i) {
		Unit* unit = (*units)[i];
		std::vector<Entity*>* neighbours = envStrategy->getNeighbours(unit, unit->getMaxSeparationDistance());//TODO przypisac na jedna klatke
		std::vector<Entity*>* buildings = envStrategy->getBuildings(unit, unit->getMaxSeparationDistance());//TODO jakis inny parametr niz max separaatino dist

		Vector3* sepPedestrian = forceStrategy->separationUnits(unit, neighbours);
		Vector3* sepObstacle = forceStrategy->separationObstacle(unit, buildings);

		Vector3* destForce = forceStrategy->destination(unit);
		Vector3* rand = forceStrategy->randomForce();

		Vector3* forces = new Vector3((*sepPedestrian) += (*sepObstacle) += (*destForce) += (*rand));
		delete sepPedestrian;
		delete sepObstacle;
		delete destForce;
		delete rand;

		unit->setAcceleration(forces);

		delete neighbours;
		delete buildings;
	}
}
