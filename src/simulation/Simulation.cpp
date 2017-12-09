#include "Simulation.h"
#include <ctime>
#include "Game.h"
#include "scene/load/SceneLoader.h"
#include "commands/creation/CreationCommandList.h"


Simulation::Simulation(Enviroment* _enviromentStrategy, CreationCommandList* _simCommandList) {
	enviroment = _enviromentStrategy;
	force = new Force();
	simObjectManager = _simCommandList->getManager();
	srand(time(NULL));
	animate = true;
	simCommandList = _simCommandList;
	aimContainer = new AimContainer();
	simulationInfo = new SimulationInfo();
	actionCommandList = new ActionCommandList(aimContainer);
	Game::get()->setActionCommmandList(actionCommandList);

	units = simObjectManager->getUnits();
	buildings = simObjectManager->getBuildings();
	resources = simObjectManager->getResources();
}

Simulation::~Simulation() {
	delete simulationInfo;
	delete aimContainer;
	delete actionCommandList;
}

void Simulation::tryToAttack(vector<Unit*>::value_type unit) {
	if (unit->hasEnemy()) {
		unit->toAttack();
	} else {
		std::vector<Unit*>* enemies = enviroment->getNeighboursFromTeam(unit, 12, unit->getTeam(), NOT_EQUAL);

		unit->toAttack(enemies);
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

void Simulation::selfAI() {
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

void Simulation::createUnits(SceneLoader* loader) {
	dbload_container* data = loader->getData();
	for (auto unit : *data->units) {
		simObjectManager->load(unit);
	}
	for (auto resource : *data->resource_entities) {
		simObjectManager->load(resource);
	}
	for (auto building : *data->buildings) {
		simObjectManager->load(building);
	}


//	simCommandList->addUnits(UNITS_NUMBER / 5, 0, new Vector3(10, 0, 10), 0);
//	simCommandList->addUnits(UNITS_NUMBER / 5, 1, new Vector3(10, 0, -10), 0);
//	simCommandList->addUnits(UNITS_NUMBER / 5, 2, new Vector3(-10, 0, 10), 0);
//	simCommandList->addUnits(UNITS_NUMBER / 5, 4, new Vector3(-10, 0, -10), 0);
//	simCommandList->addUnits(UNITS_NUMBER / 5, 5, new Vector3(0, 0, 0), 0);
//
//	simCommandList->addUnits(UNITS_NUMBER, 3, new Vector3(-50, 0, -50), 1);
//
//	simCommandList->addResource(GOLD, new Vector3(-50, 0, 45));
//	simCommandList->addResource(STONE, new Vector3(50, 0, 25));
//	simCommandList->addResource(WOOD, new Vector3(40, 0, 0));
//	simCommandList->addResource(STONE, new Vector3(0, 0, 0));
//
//
//	simCommandList->addBuilding(0, new Vector3(40, 0, 30), 0);
//	simCommandList->addBuilding(1, new Vector3(30, 0, 30), 0);
//	simCommandList->addBuilding(2, new Vector3(20, 0, 30), 0);
//	simCommandList->addBuilding(3, new Vector3(10, 0, 30), 0);
//	simCommandList->addBuilding(4, new Vector3(0, 0, 30), 0);
//	simCommandList->addBuilding(5, new Vector3(-10, 0, 30), 1);
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
		Vector3* pos = unit->getPosition();//TODO to przeniesc do mova? to moze byc [rpblem gdy jest przesuwanie poza klatk¹
		double y = enviroment->getGroundHeightAt(pos->x_, pos->z_);
		unit->updateHeight(y, maxTimeFrame);
	}
}

void Simulation::updateBuildingQueue() {
	for (Building* build : (*buildings)) {
		QueueElement* done = build->updateQueue(maxTimeFrame);
		if (done) {
			simCommandList->add(new CreationCommand(done->getType(), done->getAmount(), done->getSubtype(),
			                                        new Vector3(*(build->getTarget())), 0));
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
	simObjectManager->clearUnitsToAdd();
}

void Simulation::dispose() {
	simObjectManager->dispose();
}

void Simulation::save(SceneSaver* saver) {
	saver->saveUnits(units);
	saver->saveBuildings(buildings);
	saver->saveResourceEntities(resources);
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
				selfAI();
				actionCommandList->execute();
			}
			enviroment->update(units);
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

void Simulation::initScene(SceneLoader* loader) {
	createUnits(loader);
	simCommandList->execute();
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
		Vector3* validPos = enviroment->validatePosition(unit->getPosition());
		if (!validPos) {
			std::vector<Unit*>* neighbours = enviroment->getNeighbours(unit, unit->getMaxSeparationDistance());
			Vector3* sepPedestrian = force->separationUnits(unit, neighbours);

			Vector3* destForce = force->destination(unit);

			(*sepPedestrian) += (*destForce);
			unit->setAcceleration(sepPedestrian);

			delete destForce;
		} else {
			(*validPos) *= 20;
			unit->setAcceleration(validPos);
		}
	}
}
