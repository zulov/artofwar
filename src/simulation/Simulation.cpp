#include "Simulation.h"
#include "Game.h"
#include "commands/creation/CreationCommand.h"
#include "commands/creation/CreationCommandList.h"
#include "commands/upgrade/UpgradeCommand.h"
#include "player/PlayersManager.h"
#include "scene/load/SceneLoader.h"
#include "simulation/formation/FormationManager.h"
#include <ctime>


Simulation::Simulation(Enviroment* _enviroment, CreationCommandList* _creationCommandList) {
	enviroment = _enviroment;
	simObjectManager = _creationCommandList->getManager();
	creationCommandList = _creationCommandList;
	levelsCommandList = new UpgradeCommandList(simObjectManager);

	srand(time(NULL));

	simulationInfo = new SimulationInfo();
	actionCommandList = new CommandList();
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
	for (auto unit : *units) {
		if ((unit->getState() == UnitStateType::STOP
				|| unit->getState() == UnitStateType::MOVE)
			&& unit->getFormation() == -1) {
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

void Simulation::loadEntities(SceneLoader& loader) const {
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
	if constexpr (UNITS_NUMBER > 0) {
		simObjectManager->addUnits(UNITS_NUMBER, 0, Vector2(-20, 0), 0, 0);
		//simObjectManager->addUnits(UNITS_NUMBER, 3, Vector2(20, 0), 1, 0);
	}
}

void Simulation::loadEntities(NewGameForm* form) {
	for (const auto& player : form->players) {
		simObjectManager->addUnits(10, 1, Vector2(), player.id, 0);
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
	for (auto unit : *units) {
		unit->applyForce(maxTimeFrame);
		Vector3* pos = unit->getPosition(); //TODO to przeniesc do mova? to moze byc [rpblem gdy jest przesuwanie poza klatk¹
		const float y = enviroment->getGroundHeightAt(pos->x_, pos->z_);
		unit->updateHeight(y, maxTimeFrame);
	}
}

void Simulation::levelUp(QueueElement* done) {
	levelsCommandList->add(new UpgradeCommand(
	                                          Game::get()->getPlayersManager()->getActivePlayer()->getId(),
	                                          done->getId(),
	                                          done->getType()
	                                         ));
}

void Simulation::updateBuildingQueues(const float time) {
	for (auto build : *buildings) {
		QueueElement* done = build->updateQueue(time);
		if (done) {
			switch (done->getType()) {
			case ActionType::UNIT:
				creationCommandList->add(new CreationCommand(
				                                             ObjectType::UNIT,
				                                             done->getAmount(),
				                                             done->getId(),
				                                             build->getTarget(),
				                                             build->getPlayer(),
				                                             Game::get()->getPlayersManager()->getPlayer(build->getPlayer())->
				                                                          getLevelForUnit(done->getId())
				                                            ));
				break;
			case ActionType::UNIT_LEVEL:
			case ActionType::BUILDING_LEVEL:
			case ActionType::UNIT_UPGRADE:
				levelUp(done);
				break;
			}

			delete done;
		}
	}
}

void Simulation::updateQueues() {
	updateBuildingQueues(maxTimeFrame);
	QueueElement* done = Game::get()->getQueueManager()->update(maxTimeFrame);
	if (done) {
		switch (done->getType()) {
		case ActionType::BUILDING_LEVEL:
			levelUp(done);
			break;
		}
		delete done;
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

void Simulation::changeCoef(int i, int wheel) {
	force.changeCoef(i, wheel);
}

void Simulation::changeColorMode(ColorMode _colorMode) {
	currentColor = _colorMode;
}

void Simulation::performStateAction() {
	for (auto unit : *units) {
		unit->executeState();
	}
}

void Simulation::handleTimeInFrame(float timeStep) {
	countFrame();
	moveUnits(maxTimeFrame - (accumulateTime - timeStep));
	accumulateTime -= maxTimeFrame;
}

SimulationInfo* Simulation::update(float timeStep) {
	simulationInfo->reset();
	timeStep = updateTime(timeStep);
	if (accumulateTime >= maxTimeFrame) {
		handleTimeInFrame(timeStep);
		if (currentFrameNumber % 3 == 0) {
			executeLists();
			selfAI();
		}
		actionCommandList->execute();
		enviroment->update(units);

		calculateForces();
		applyForce();

		moveUnitsAndCheck(accumulateTime);

		performStateAction();
		updateQueues();

		simObjectManager->prepareToDispose();
		simObjectManager->updateInfo(simulationInfo);

		Game::get()->getFormationManager()->update();
	} else {
		moveUnits(timeStep);
	}
	return simulationInfo;
}

void Simulation::executeLists() {
	levelsCommandList->execute();
	creationCommandList->execute();
}

void Simulation::initScene(SceneLoader& loader) {
	//loadEntities(loader);
	addTestEntities();
	executeLists();
}

void Simulation::initScene(NewGameForm* form) {
	loadEntities(form);
	addTestEntities();
	executeLists();
}

void Simulation::moveUnits(const float timeStep) {
	for (auto unit : *units) {
		unit->move(timeStep);
	}
}

void Simulation::moveUnitsAndCheck(const float timeStep) {
	for (auto unit : *units) {
		unit->move(timeStep);
		unit->checkAim();
		unit->changeColor(currentColor);
	}
}

void Simulation::calculateForces() {
	for (auto unit : *units) {
		Vector2 newForce;

		std::vector<Unit*>* neighbours = enviroment->getNeighbours(unit, unit->getMaxSeparationDistance());
		Vector2 repulse = enviroment->repulseObstacle(unit);

		force.separationUnits(newForce, unit, neighbours);
		force.separationObstacle(newForce, unit, repulse);
		force.destination(newForce, unit);
		force.formation(newForce, unit);
		force.escapeFromInvalidPosition(newForce, enviroment->validatePosition(unit->getPosition()));

		float* stats = force.stats();

		unit->setAcceleration(newForce);
	}
}
