#include "Simulation.h"


Simulation::Simulation(EnviromentStrategy* _enviromentStrategy, SimulationCommandList* _simCommandList,
                       SimulationObjectManager* _simObjectManager) {
	envStrategy = _enviromentStrategy;
	forceStrategy = new ForceStrategy();
	simObjectManager = _simObjectManager;
	srand(time(NULL));
	animate = true;
	simCommandList = _simCommandList;
	aimContainer = new AimContainer();
	actionCommandList = new ActionCommandList(aimContainer);
	Game::get()->setActionCommmandList(actionCommandList);
	simulationInfo = new SimulationInfo();
	createUnits();

	units = simObjectManager->getUnits();
	buildings = simObjectManager->getBuildings();
	resources = simObjectManager->getResources();
}

void Simulation::selftAI() {
	for (auto unit : (*units)) {
		if (unit->checkTransition(UnitStateType::ATTACK)) {
			if (unit->hasEnemy()) {
				unit->attack();
			} else {
				std::vector<Physical*>* enemies = envStrategy->getNeighboursFromTeam(unit, 12, unit->getTeam(), NOT_EQUAL);

				unit->attack(enemies);
				delete enemies;
			}
		}
	}
}

void Simulation::createUnits() {
	simCommandList->add(new SimulationCommand(UNIT, UNITS_NUMBER / 5, 0, new Vector3(10, 0, 10), CONSTANT, 0));
	simCommandList->add(new SimulationCommand(UNIT, UNITS_NUMBER / 5, 1, new Vector3(10, 0, -10), CONSTANT, 0));
	simCommandList->add(new SimulationCommand(UNIT, UNITS_NUMBER / 5, 2, new Vector3(-10, 0, 10), CONSTANT, 0));
	simCommandList->add(new SimulationCommand(UNIT, UNITS_NUMBER / 5, 4, new Vector3(-10, 0, -10), CONSTANT, 0));
	simCommandList->add(new SimulationCommand(UNIT, UNITS_NUMBER / 5, 5, new Vector3(0, 0, 0), CONSTANT, 0));

	simCommandList->add(new SimulationCommand(UNIT, UNITS_NUMBER, 3, new Vector3(-50, 0, -50), CONSTANT, 1));

	simCommandList->add(new SimulationCommand(RESOURCE, 4, GOLD, new Vector3(-50, 0, 45), CONSTANT, 1));
	simCommandList->add(new SimulationCommand(RESOURCE, 4, STONE, new Vector3(50, 0, 25), CONSTANT, 1));
	simCommandList->add(new SimulationCommand(RESOURCE, 9, WOOD, new Vector3(40, 0, 0), CONSTANT, 1));
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
	for (auto unit : (*units)) {
		unit->applyForce(maxTimeFrame);
		Vector3* pos = unit->getPosition();//TODO to przeniesc do mova?
		double y = envStrategy->getGroundHeightAt(pos->x_, pos->z_);
		unit->updateHeight(y, maxTimeFrame);
	}
}

void Simulation::updateBuildingQueue() {
	for (Building* build : (*buildings)) {
		QueueElement* done = build->updateQueue(maxTimeFrame);
		if (done) {
			simCommandList->add(new SimulationCommand(done->getType(), done->getAmount(), done->getSubtype(),
			                                          new Vector3(*(build->getTarget())), SpacingType::CONSTANT, 0));
		}
	}
}

int Simulation::getUnitsNumber() {
	return units->size();
}

SimulationInfo* Simulation::getInfo() {
	return simulationInfo;
}

void Simulation::updateEnviroment() {
	if (simulationInfo->ifAmountBuildingChanged()) {
		envStrategy->update(buildings);
	}
	if (simulationInfo->ifAmountResourceChanged()) {
		envStrategy->update(resources);
	}
}

void Simulation::dispose() {
	simObjectManager->dispose();
}

void Simulation::performAction() {
	for (auto unit : (*units)) {
		unit->executeState();
	}
}

void Simulation::update(Input* input, float timeStep) {
	if (input->GetKeyPress(KEY_SPACE)) {
		animate = !animate;
	}

	if (animate) {
		simulationInfo->reset();
		timeStep = updateTime(timeStep);
		if (accumulateTime >= maxTimeFrame) {
			countFrame();

			moveUnits(maxTimeFrame - (accumulateTime - timeStep));
			accumulateTime -= maxTimeFrame;
			if (currentFrameNumber % 3 == 0) {
				simCommandList->execute();
				selftAI();
				actionCommandList->execute();
			}
			envStrategy->update(units);
			simObjectManager->clean();
			simObjectManager->updateInfo(simulationInfo);
			updateEnviroment();

			calculateForces();
			applyForce();

			moveUnitsAndCheck(accumulateTime);

			performAction();
			updateBuildingQueue();
		} else {
			moveUnits(timeStep);
		}
	}
}

void Simulation::moveUnits(float timeStep) {
	for (auto unit : (*units)) {
		unit->move(timeStep);
	}
}

void Simulation::moveUnitsAndCheck(float timeStep) {
	for (auto unit : (*units)) {
		unit->move(timeStep);
		unit->checkAim();
	}
	aimContainer->clean();
}

void Simulation::calculateForces() {
	for (auto unit : (*units)) {
		std::vector<Physical*>* neighbours = envStrategy->getNeighbours(unit, unit->getMaxSeparationDistance());
		//std::vector<Physical*>* buildings = envStrategy->getBuildings(unit, unit->getMaxSeparationDistance());//TODO jakis inny parametr niz max separaatino dist
		Vector2 repulsive = envStrategy->getRepulsiveAt(unit->getPosition());

		Vector3* sepPedestrian = forceStrategy->separationUnits(unit, neighbours);
		Vector3* sepObstacle = forceStrategy->separationObstacle(unit, repulsive);

		Vector3* destForce = forceStrategy->destination(unit);

		(*sepPedestrian) += (*sepObstacle) += (*destForce);
		unit->setAcceleration(sepPedestrian);

		delete sepObstacle;
		delete destForce;
		delete neighbours;
		//delete buildings;
	}
}
