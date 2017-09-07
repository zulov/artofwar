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
	simulationInfo = new SimulationInfo();
	createUnits();
}

void Simulation::action() {
	for (unsigned i = 0; i < units->size(); ++i) {
		Unit* unit = (*units)[i];
		if (unit->getState() == US_STOP || unit->getState() == US_ATTACK) {
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
	simCommandList->add(new SimulationCommand(ObjectType::UNIT, UNITS_NUMBER, 0, new Vector3(0, 0, 0), SpacingType::CONSTANT, 0));
	simCommandList->add(new SimulationCommand(ObjectType::UNIT, UNITS_NUMBER, 1, new Vector3(-50, 0, -50), SpacingType::CONSTANT, 1));

	simCommandList->add(new SimulationCommand(ObjectType::RESOURCE, 4, ResourceType::GOLD, new Vector3(-50, 0, 45), SpacingType::CONSTANT, 1));
	simCommandList->add(new SimulationCommand(ObjectType::RESOURCE, 4, ResourceType::STONE, new Vector3(50, 0, 25), SpacingType::CONSTANT, 1));
	simCommandList->add(new SimulationCommand(ObjectType::RESOURCE, 9, ResourceType::WOOD, new Vector3(40, 0, 0), SpacingType::CONSTANT, 1));
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
		unit->updateRotation();
		Vector3* pos = unit->getPosition();
		double y = envStrategy->getGroundHeightAt(pos->x_, pos->z_);
		unit->updateHeight(y, maxTimeFrame);
	}
}

void Simulation::updateBuildingQueue() {
	for (Building* build : (*buildings)) {
		QueueElement* done = build->updateQueue(maxTimeFrame);
		if (done) {
			simCommandList->add(new SimulationCommand(done->getType(), done->getAmount(), done->getSubtype(), new Vector3(*(build->getTarget())), SpacingType::CONSTANT, 0));
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
	envStrategy->update(units);
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

void Simulation::update(Input* input, float timeStep) {
	if (input->GetKeyPress(KEY_SPACE)) {
		animate = !animate;
	}

	if (animate) {
		simulationInfo->reset();
		timeStep = updateTime(timeStep);
		if (accumulateTime >= maxTimeFrame) {
			countFrame();
			double diff = maxTimeFrame - (accumulateTime - timeStep);
			moveUnits(diff);
			accumulateTime -= maxTimeFrame;
			if (currentFrameNumber % 3 == 0) {
				simCommandList->execute();
				aimContainer->clean();
				action();
				actionCommandList->execute();
			}
			simObjectManager->clean();
			simObjectManager->updateInfo(simulationInfo);
			updateEnviroment();

			units = simObjectManager->getUnits();//TOdo te linijki sa pewnie nie potrzebne
			buildings = simObjectManager->getBuildings();
			resources = simObjectManager->getResources();

			updateBuildingQueue();

			calculateForces();
			applyForce();

			timeStep = accumulateTime;
			moveUnitsAndCheck(timeStep);
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
}

void Simulation::calculateForces() {
	for (auto unit : (*units)) {
		std::vector<Physical*>* neighbours = envStrategy->getNeighbours(unit, unit->getMaxSeparationDistance());
		//std::vector<Physical*>* buildings = envStrategy->getBuildings(unit, unit->getMaxSeparationDistance());//TODO jakis inny parametr niz max separaatino dist
		Vector3* repulsive = envStrategy->getRepulsiveAt(unit->getPosition());

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
