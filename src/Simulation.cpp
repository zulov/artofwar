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
		std::vector<Physical*>* enemies = envStrategy->getNeighboursFromTeam(unit, 12, unit->getTeam(), NOT_EQUAL);

		unit->attack(enemies);
		delete enemies;
	}
}

void Simulation::createUnits() {
	simCommandList->add(new SimulationCommand(UNITS_NUMBER, UnitType::WARRIOR, new Vector3(0, 0, 0), SpacingType::CONSTANT, 0));
	simCommandList->add(new SimulationCommand(UNITS_NUMBER, UnitType::ARCHER, new Vector3(-50, 0, -50), SpacingType::CONSTANT, 1));

	simCommandList->add(new SimulationCommand(4, ResourceType::GOLD, new Vector3(-50, 0, 35), SpacingType::CONSTANT, 1));
	simCommandList->add(new SimulationCommand(4, ResourceType::STONE, new Vector3(40, 0, 25), SpacingType::CONSTANT, 1));
	simCommandList->add(new SimulationCommand(9, ResourceType::WOOD, new Vector3(30, 0, 0), SpacingType::CONSTANT, 1));
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
	if (currentFrameNumber >= framesPeriod) {
		currentFrameNumber = 0;
	}
}

void Simulation::applyForce() {
	for (unsigned i = 0; i < units->size(); ++i) {
		Unit* unit = (*units)[i];
		unit->applyForce(maxTimeFrame);
		unit->updateRotation();
		Vector3* pos = unit->getPosition();
		double y = envStrategy->getGroundHeightAt(pos->x_, pos->z_);
		unit->updateHeight(y, maxTimeFrame);
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
			if (currentFrameNumber % 3 == 0) {
				simCommandList->execute();
				actionCommandList->execute();
				aimContainer->clean();
				action();
			}

			units = simObjectManager->getUnits();
			buildings = simObjectManager->getBuildings();
			resources = simObjectManager->getResources();
			envStrategy->update(units);
			envStrategy->update(buildings);
			envStrategy->update(resources);

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
		std::vector<Physical*>* neighbours = envStrategy->getNeighbours(unit, unit->getMaxSeparationDistance());
		//std::vector<Physical*>* buildings = envStrategy->getBuildings(unit, unit->getMaxSeparationDistance());//TODO jakis inny parametr niz max separaatino dist
		Vector3 * repulsive = envStrategy->getRepulsiveAt(unit->getPosition());

		Vector3* sepPedestrian = forceStrategy->separationUnits(unit, neighbours);
		Vector3* sepObstacle = forceStrategy->separationObstacle(unit, repulsive);

		Vector3* destForce = forceStrategy->destination(unit);
		Vector3* rand = forceStrategy->randomForce();

		(*sepPedestrian) += (*sepObstacle) += (*destForce) += (*rand);
		unit->setAcceleration(sepPedestrian);

		delete sepObstacle;
		delete destForce;
		delete rand;
		delete neighbours;
		//delete buildings;
	}
}
