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
	for (unsigned i = 0; i < units->size(); ++i) {
		Unit* unit = (*units)[i];
		std::vector<Entity*>* enemies = envStrategy->getNeighboursFromTeam(unit, 6, unit->getTeam(), NOT_EQUAL);

		unit->attack(enemies);
		delete enemies;
	}
}

void Simulation::createUnits() {
	simCommandList->add(new SimulationCommand(UNITS_NUMBER, UnitType::WARRIOR, new Vector3(0, 0, 0), SpacingType::CONSTANT, 0));
	simCommandList->add(new SimulationCommand(UNITS_NUMBER, UnitType::ARCHER, new Vector3(-50, 0, -50), SpacingType::CONSTANT, 1));
}


void Simulation::update(Input* input, float timeStep) {
	if (input->GetKeyPress(KEY_SPACE)) {
		animate = !animate;
	}

	if (animate) {
		
		if (timeStep > maxTimeFrame) {
			timeStep = maxTimeFrame;
		}
		accumulateTime += timeStep;
		units = simObjectManager->getUnits();
		buildings = simObjectManager->getBuildings();

		if (accumulateTime >= maxTimeFrame) {
			double diff = maxTimeFrame - (accumulateTime - timeStep);
			moveUnits(diff);
			accumulateTime -= maxTimeFrame;

			simCommandList->execute();
			actionCommandList->execute();


			envStrategy->update(units);
			envStrategy->update(buildings);
			calculateForces();
			for (unsigned i = 0; i < units->size(); ++i) {
				Unit* unit = (*units)[i];
				unit->applyForce(maxTimeFrame);
			}
			action(timeStep);
			timeStep = accumulateTime;
		}

		moveUnits(timeStep);

		aimContainer->clean();
	}
}

void Simulation::moveUnits(float timeStep) {
	for (unsigned i = 0; i < units->size(); ++i) {
		Unit* unit = (*units)[i];
		unit->move(timeStep);
	}
}

void Simulation::calculateForces() {
	for (unsigned i = 0; i < units->size(); ++i) {
		Unit* unit = (*units)[i];
		std::vector<Entity*>* neighbours = envStrategy->getNeighbours(unit, unit->getMaxSeparationDistance());
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
