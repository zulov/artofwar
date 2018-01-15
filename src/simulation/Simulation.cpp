#include "Simulation.h"
#include "Game.h"
#include "commands/creation/CreationCommand.h"
#include "commands/creation/CreationCommandList.h"
#include "scene/load/SceneLoader.h"
#include <ctime>


Simulation::Simulation(Enviroment* _enviroment, CreationCommandList* _simCommandList) {
	enviroment = _enviroment;
	simObjectManager = _simCommandList->getManager();
	simCommandList = _simCommandList;

	srand(time(NULL));
	animate = true;

	aimContainer = new AimContainer();
	simulationInfo = new SimulationInfo();
	actionCommandList = new ActionCommandList(aimContainer);
	Game::get()->setActionCommmandList(actionCommandList);

	units = simObjectManager->getUnits();
	buildings = simObjectManager->getBuildings();
	resources = simObjectManager->getResources();
}

Simulation::~Simulation() {
	delete aimContainer;
	delete simulationInfo;
	delete actionCommandList;
	Game::get()->setActionCommmandList(nullptr);
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

void Simulation::loadEntities(SceneLoader& loader) {
	dbload_container* data = loader.getData();
	for (auto unit : *data->units) {
		simObjectManager->load(unit);
	}
	for (auto resource : *data->resource_entities) {
		simObjectManager->load(resource);
	}
	for (auto building : *data->buildings) {
		simObjectManager->load(building);
	}
}

void Simulation::loadEntities(NewGameForm* form) {
	for (auto player : form->players) {
		simObjectManager->addUnits(10, 1, new Vector3(), player.id);
	}
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
		Vector3* pos = unit->getPosition(); //TODO to przeniesc do mova? to moze byc [rpblem gdy jest przesuwanie poza klatk¹
		double y = enviroment->getGroundHeightAt(pos->x_, pos->z_);
		unit->updateHeight(y, maxTimeFrame);
	}
}

void Simulation::updateBuildingQueue() {
	for (Building* build : (*buildings)) {
		QueueElement* done = build->updateQueue(maxTimeFrame);
		if (done) {
			simCommandList->add(new CreationCommand(done->getType(), done->getAmount(), done->getSubtype(),
			                                        new Vector3(build->getTarget()), 0));
			delete done;
		}
	}
}

int Simulation::getUnitsNumber() {
	return units->size();
}

SimulationInfo* Simulation::getInfo() {
	return simulationInfo;
}

void Simulation::dispose() {
	simObjectManager->dispose();
}

void Simulation::save(SceneSaver& saver) {
	saver.saveUnits(units);
	saver.saveBuildings(buildings);
	saver.saveResourceEntities(resources);
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

void Simulation::initScene(SceneLoader& loader) {
	loadEntities(loader);
	simCommandList->execute();
}

void Simulation::initScene(NewGameForm* form) {
	loadEntities(form);
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

			Vector3* sepPedestrian = force.separationUnits(unit, neighbours);
			Vector3* destForce = force.destination(unit);

			(*sepPedestrian) += (*destForce);
			unit->setAcceleration(sepPedestrian);

			delete sepPedestrian;
			delete destForce;
		} else {
			(*validPos) *= 20;
			unit->setAcceleration(validPos);
			delete validPos;
		}
	}
}
