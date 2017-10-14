#include "Simulation.h"


Simulation::Simulation(Enviroment* _enviromentStrategy, SimulationCommandList* _simCommandList,
                       SimulationObjectManager* _simObjectManager) {
	envStrategy = _enviromentStrategy;
	forceStrategy = new Force();
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

void Simulation::tryToAttack(vector<Unit*>::value_type unit) {
	if (unit->hasEnemy()) {
		unit->toAttack();
	} else {
		std::vector<Unit*>* enemies = envStrategy->getNeighboursFromTeam(unit, 12, unit->getTeam(), NOT_EQUAL);

		unit->toAttack(enemies);
		delete enemies;
	}
}

void Simulation::tryToCollect(Unit* unit) {
	if (unit->hasResource()) {
		unit->toCollect();
	} else {
		//		std::vector<Physical*>* resources = envStrategy->getResources(unit, 12);
		//
		//		unit->toCollect(resources);
		//		delete resources;
	}
}

void Simulation::selftAI() {
	for (auto unit : (*units)) {
		if (unit->getState() == UnitStateType::STOP || unit->getState() == UnitStateType::MOVE) {
			if (unit->checkTransition(unit->getActionState())) {
				switch (unit->getActionState()) {
				case UnitStateType::ATTACK:
					tryToAttack(unit);
					break;
				case UnitStateType::COLLECT:
					tryToCollect(unit);
					break;
				default: ;
				}
			}
		}

	}
}

void Simulation::createUnits() {
	simCommandList->add(new CreationCommand(UNIT, UNITS_NUMBER / 5, 0, new Vector3(10, 0, 10), CONSTANT, 0));
	simCommandList->add(new CreationCommand(UNIT, UNITS_NUMBER / 5, 1, new Vector3(10, 0, -10), CONSTANT, 0));
	simCommandList->add(new CreationCommand(UNIT, UNITS_NUMBER / 5, 2, new Vector3(-10, 0, 10), CONSTANT, 0));
	simCommandList->add(new CreationCommand(UNIT, UNITS_NUMBER / 5, 4, new Vector3(-10, 0, -10), CONSTANT, 0));
	simCommandList->add(new CreationCommand(UNIT, UNITS_NUMBER / 5, 5, new Vector3(0, 0, 0), CONSTANT, 0));

	simCommandList->add(new CreationCommand(UNIT, UNITS_NUMBER, 3, new Vector3(-50, 0, -50), CONSTANT, 1));

	simCommandList->add(new CreationCommand(RESOURCE, 4, GOLD, new Vector3(-50, 0, 45), CONSTANT, 1));
	simCommandList->add(new CreationCommand(RESOURCE, 4, STONE, new Vector3(50, 0, 25), CONSTANT, 1));
	simCommandList->add(new CreationCommand(RESOURCE, 9, WOOD, new Vector3(40, 0, 0), CONSTANT, 1));
	simCommandList->add(new CreationCommand(RESOURCE, 1, STONE, new Vector3(0, 0, 0), CONSTANT, 1));


	simCommandList->add(new CreationCommand(BUILDING, 1, 0, new Vector3(40, 0, 30), CONSTANT, 0));
	simCommandList->add(new CreationCommand(BUILDING, 1, 1, new Vector3(30, 0, 30), CONSTANT, 0));
	simCommandList->add(new CreationCommand(BUILDING, 1, 2, new Vector3(20, 0, 30), CONSTANT, 0));
	simCommandList->add(new CreationCommand(BUILDING, 1, 3, new Vector3(10, 0, 30), CONSTANT, 0));
	simCommandList->add(new CreationCommand(BUILDING, 1, 4, new Vector3(0, 0, 30), CONSTANT, 0));
	simCommandList->add(new CreationCommand(BUILDING, 1, 5, new Vector3(-10, 0, 30), CONSTANT, 1));
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
			simCommandList->add(new CreationCommand(done->getType(), done->getAmount(), done->getSubtype(),
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
		envStrategy->update(simObjectManager->getBuildingsToAdd());
		simObjectManager->clearBuildingsToAdd();
	}
	if (simulationInfo->ifAmountResourceChanged()) {
		envStrategy->update(simObjectManager->getResourcesToAdd());
		simObjectManager->clearResourcesToAdd();
	}
	simObjectManager->clearUnitsToAdd();
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
		Vector3* validPos = envStrategy->validatePosition(unit->getPosition());
		if (!validPos) {
			std::vector<Unit*>* neighbours = envStrategy->getNeighbours(unit, unit->getMaxSeparationDistance());
			Vector3* sepPedestrian = forceStrategy->separationUnits(unit, neighbours);

			Vector3* destForce = forceStrategy->destination(unit);

			(*sepPedestrian) += (*destForce);
			unit->setAcceleration(sepPedestrian);

			delete destForce;
			delete neighbours;
		} else {
			(*validPos) *= 20;
			unit->setAcceleration(validPos);
		}


	}
}
