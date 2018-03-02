#include "Simulation.h"
#include "Game.h"
#include "commands/creation/CreationCommand.h"
#include "commands/creation/CreationCommandList.h"
#include "commands/upgrade/UpgradeCommand.h"
#include "scene/load/SceneLoader.h"
#include <ctime>
#include "player/PlayersManager.h"


Simulation::Simulation(Enviroment* _enviroment, CreationCommandList* _simCommandList) {
	enviroment = _enviroment;
	simObjectManager = _simCommandList->getManager();
	creationCommandList = _simCommandList;
	levelsCommandList = new CommandList();

	srand(time(NULL));

	simulationInfo = new SimulationInfo();
	actionCommandList = new ActionCommandList();
	Game::get()->setActionCommmandList(actionCommandList);

	units = simObjectManager->getUnits();
	buildings = simObjectManager->getBuildings();
	resources = simObjectManager->getResources();
}

Simulation::~Simulation() {
	delete simulationInfo;
	delete actionCommandList;
	delete levelsCommandList;
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

void Simulation::addTestEntities() {
	if (UNITS_NUMBER > 0) {
		simObjectManager->addUnits(UNITS_NUMBER, 0, new Vector3(), 0, 0);
	}
}

void Simulation::loadEntities(NewGameForm* form) {
	for (auto player : form->players) {
		simObjectManager->addUnits(10, 1, new Vector3(), player.id, 0);
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
			switch (done->getType()) {
			case QueueType::UNIT:
				creationCommandList->add(new CreationCommand(
				                                             ObjectType::UNIT,
				                                             done->getAmount(),
				                                             done->getId(),
				                                             new Vector3(build->getTarget()),
				                                             build->getPlayer(),
				                                             Game::get()->getPlayersManager()->getPlayer(build->getPlayer())->
				                                                          getLevelForUnit(done->getId())
				                                            ));
				break;
			case QueueType::UNIT_LEVEL:
				levelsCommandList->add(new UpgradeCommand(
				                                          Game::get()->getPlayersManager()->getActivePlayer()->getId(),
				                                          done->getId(),
				                                          done->getType()
				                                         ));
				break;
			case QueueType::BUILDING_LEVEL:
				levelsCommandList->add(new UpgradeCommand(
				                                          Game::get()->getPlayersManager()->getActivePlayer()->getId(),
				                                          done->getId(),
				                                          done->getType()
				                                         ));
				break;
			default: ;
			}

			delete done;
		}
	}
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

SimulationInfo* Simulation::update(float timeStep) {

	simulationInfo->reset();
	timeStep = updateTime(timeStep);
	if (accumulateTime >= maxTimeFrame) {
		countFrame();

		moveUnits(maxTimeFrame - (accumulateTime - timeStep));
		accumulateTime -= maxTimeFrame;
		if (currentFrameNumber % 3 == 0) {
			levelsCommandList->execute();
			creationCommandList->execute();
			selfAI();
			actionCommandList->execute();
		}
		enviroment->update(units);

		calculateForces();
		applyForce();

		moveUnitsAndCheck(accumulateTime);

		performAction();
		updateBuildingQueue();

		simObjectManager->prepareToDispose();
		simObjectManager->updateInfo(simulationInfo);

		simulationInfo->setUnitsNumber(units->size());
	} else {
		moveUnits(timeStep);
	}
	return simulationInfo;
}

void Simulation::initScene(SceneLoader& loader) {
	loadEntities(loader);
	addTestEntities();
	levelsCommandList->execute();
	creationCommandList->execute();
}

void Simulation::initScene(NewGameForm* form) {
	loadEntities(form);
	addTestEntities();
	levelsCommandList->execute();
	creationCommandList->execute();
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
