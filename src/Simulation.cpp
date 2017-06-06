#include "Simulation.h"


Simulation::Simulation(EnviromentStrategy* _enviromentStrategy, SimulationCommandList* _simCommandList, SimulationObjectManager* _simObjectManager) {
	envStrategy = _enviromentStrategy;
	forceStrategy = new ForceStrategy();
	simObjectManager = _simObjectManager;
	units = simObjectManager->getUnits();
	buildings = simObjectManager->getBuildings();
	srand(time(NULL));
	animate = true;
	simCommandList = _simCommandList;
	aimContainer = new AimContainer();
	actionCommandList = new ActionCommandList(aimContainer);
	Game::get()->setActionCommmandList(actionCommandList);
	createUnits();
}

void Simulation::action() {
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


float Simulation::updateTime(float timeStep) {
	if (timeStep > maxTimeFrame) {
		timeStep = maxTimeFrame;
	}
	accumulateTime += timeStep;
	return timeStep;
}

void Simulation::countFrame() {
	++currentFrameNumber;
	if(currentFrameNumber>= framesPeriod) {
		currentFrameNumber = 0;
	}
}

void Simulation::applyForce() {
	for (unsigned i = 0; i < units->size(); ++i) {
		Unit* unit = (*units)[i];
		unit->applyForce(maxTimeFrame);
	}
}

void Simulation::update(Input* input, float timeStep) {
	if (input->GetKeyPress(KEY_SPACE)) {
		animate = !animate;
	}

	if (animate) {
		timeStep = updateTime(timeStep);
		if (accumulateTime >= maxTimeFrame) {
			countFrame();
			double diff = maxTimeFrame - (accumulateTime - timeStep);
			moveUnits(diff);
			accumulateTime -= maxTimeFrame;
			if (currentFrameNumber % 3==0) {
				simCommandList->execute();
				actionCommandList->execute();
				aimContainer->clean();
				action();
			}

			units = simObjectManager->getUnits();
			buildings = simObjectManager->getBuildings();
			envStrategy->update(units);
			envStrategy->update(buildings);
			calculateForces();
			applyForce();
		
			timeStep = accumulateTime;

		}
		moveUnits(timeStep);

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
