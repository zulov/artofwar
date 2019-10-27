#include "Simulation.h"
#include "debug/DebugLineRepo.h"
#include "debug/DebugUnitType.h"
#include "Game.h"
#include "ObjectEnums.h"
#include "OperatorType.h"
#include "SimulationInfo.h"
#include "SimulationObjectManager.h"
#include "commands/creation/CreationCommand.h"
#include "commands/creation/CreationCommandList.h"
#include "commands/upgrade/UpgradeCommand.h"
#include "commands/upgrade/UpgradeCommandList.h"
#include "env/Environment.h"
#include "hud/window/main_menu/new_game/NewGameForm.h"
#include "objects/ActionType.h"
#include "objects/PhysicalUtils.h"
#include "objects/building/Building.h"
#include "objects/queue/QueueElement.h"
#include "objects/unit/ColorMode.h"
#include "objects/unit/Unit.h"
#include "objects/unit/state/StateManager.h"
#include "objects/unit/state/UnitState.h"
#include "player/Player.h"
#include "player/PlayersManager.h"
#include "scene/load/SceneLoader.h"
#include "scene/save/SceneSaver.h"
#include "simulation/formation/FormationManager.h"
#include "UnitOrder.h"
#include <ctime>
#include "player/ai/AiManager.h"


Simulation::Simulation(Environment* enviroment, CreationCommandList* creationCommandList): enviroment(enviroment),
                                                                                           creationCommandList(
	                                                                                           creationCommandList) {
	simObjectManager = creationCommandList->getManager();
	levelsCommandList = new UpgradeCommandList(simObjectManager);
	colorScheme = ColorMode::BASIC;

	srand(time(NULL));

	simulationInfo = new SimulationInfo();
	actionCommandList = new CommandList();
	Game::setActionCommandList(actionCommandList);

	units = simObjectManager->getUnits();
	buildings = simObjectManager->getBuildings();
	resources = simObjectManager->getResources();
	DebugLineRepo::init(DebugLineType::UNIT_LINES);
}

Simulation::~Simulation() {
	delete simulationInfo;
	delete actionCommandList;
	delete levelsCommandList;
	Game::setActionCommandList(nullptr);
}

SimulationInfo* Simulation::update(float timeStep) {
	simulationInfo->reset();
	timeStep = updateTime(timeStep);
	if (accumulateTime >= maxTimeFrame) {
		handleTimeInFrame(timeStep);

		executeLists();
		selfAI();
		aiPlayers();

		actionCommandList->execute();
		enviroment->update(units);
		enviroment->updateInfluence(units, buildings, resources);

		calculateForces();
		applyForce();

		moveUnitsAndCheck(accumulateTime);

		performStateAction(timeStep);
		updateQueues();

		simObjectManager->prepareToDispose();
		simObjectManager->updateInfo(simulationInfo);
		simulationInfo->setCurrentFrame(currentFrameNumber);
		enviroment->removeFromGrids(simObjectManager->getToDispose());
		Game::getPlayersMan()->update();
		Game::getFormationManager()->update();
	} else {
		moveUnits(timeStep);
	}
	return simulationInfo;
}

void Simulation::tryToAttack(Unit* unit, float dist, UnitOrder order, const std::function<bool(Physical*)>& condition) {
	toAction(unit, enviroment->getNeighboursFromTeam(unit, dist, unit->getTeam(),
	                                                 OperatorType::NOT_EQUAL), order, condition);
}

void Simulation::tryToCollect(Unit* unit) {
	toAction(unit, enviroment->getResources(unit, 12), UnitOrder::COLLECT, belowClose);
}

void Simulation::toAction(Unit* unit, std::vector<Physical*>* list, UnitOrder order,
                          const std::function<bool(Physical*)>& condition) {
	auto [closest, minDistance, indexToInteract] = unit->closestPhysical(list, condition);
	unit->toAction(closest, minDistance, indexToInteract, order);
}

void Simulation::selfAI() {
	for (auto unit : *units) {
		switch (unit->getState()) {
		case UnitState::CHARGE:
			unit->toCharge(enviroment->getNeighboursFromTeam(unit, 12, unit->getTeam(), OperatorType::NOT_EQUAL));
			break;
		case UnitState::STOP:
		case UnitState::MOVE:
			if (currentFrameNumber % 3 == 0 && unit->getFormation() == -1
				&& StateManager::checkChangeState(unit, unit->getActionState())) {
				switch (unit->getActionState()) {
				case UnitState::ATTACK:
					tryToAttack(unit, 12, UnitOrder::ATTACK, belowClose);
					break ;
				case UnitState::COLLECT:
					tryToCollect(unit);
					break;
				case UnitState::SHOT:
					tryToAttack(unit, 12, UnitOrder::ATTACK, belowRange);
					break;
				}
			}
			break;
		}
	}
}

void Simulation::loadEntities(SceneLoader& loader) const {
	for (auto unit : *loader.getData()->units) {
		simObjectManager->load(unit);
	}
	for (auto resource : *loader.getData()->resource_entities) {
		simObjectManager->load(resource);
	}
	for (auto building : *loader.getData()->buildings) {
		simObjectManager->load(building);
	}
}

void Simulation::addTestEntities() const {
	if constexpr (UNITS_NUMBER > 0) {
		//creationCommandList->addUnits(UNITS_NUMBER, 2, Urho3D::Vector2(20, -30), 0, 0);
		//creationCommandList->addUnits(UNITS_NUMBER, 0, Urho3D::Vector2(-20, -10), 1, 0);
		creationCommandList->addUnits(UNITS_NUMBER * 5, 0, Urho3D::Vector2(-20, -20), 0, 0);

		creationCommandList->addBuilding(1, Urho3D::Vector2(-10, -18), 0, 0);
		creationCommandList->addBuilding(1, Urho3D::Vector2(-5, -18), 0, 0);
		creationCommandList->addBuilding(1, Urho3D::Vector2(0, -20), 0, 0);
		creationCommandList->addBuilding(1, Urho3D::Vector2(7, -16), 0, 0);
		creationCommandList->addBuilding(1, Urho3D::Vector2(15, -18), 0, 0);


		creationCommandList->addBuilding(1, Urho3D::Vector2(-50, -30), 0, 0);
		creationCommandList->addBuilding(1, Urho3D::Vector2(-50, -28), 0, 0);
		creationCommandList->addBuilding(1, Urho3D::Vector2(-50, -26), 0, 0);
		creationCommandList->addBuilding(1, Urho3D::Vector2(-50, -24), 0, 0);
		creationCommandList->addBuilding(1, Urho3D::Vector2(-50, -22), 0, 0);

		creationCommandList->addBuilding(1, Urho3D::Vector2(-50, -18), 0, 0);

		creationCommandList->addBuilding(1, Urho3D::Vector2(-50, -14), 0, 0);
		creationCommandList->addBuilding(1, Urho3D::Vector2(-50, -12), 0, 0);
		creationCommandList->addBuilding(1, Urho3D::Vector2(-50, -10), 0, 0);
		creationCommandList->addBuilding(1, Urho3D::Vector2(-50, -8), 0, 0);

		creationCommandList->addBuilding(2, Urho3D::Vector2(-100, 50), 1, 0);

		//creationCommandList->addUnits(UNITS_NUMBER*100, 0, Urho3D::Vector2(-200, -200), 0, 0);
	}
}

void Simulation::loadEntities(NewGameForm* form) const {
	for (const auto& player : form->players) {
		simObjectManager->addUnits(10, 1, Urho3D::Vector2(), player.id, 0);
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

void Simulation::applyForce() const {
	for (auto unit : *units) {
		unit->applyForce(maxTimeFrame);
		auto pos = unit->getPosition();
		//TODO to przeniesc do mova? to moze byc [rpblem gdy jest przesuwanie poza klatk¹
		const float y = enviroment->getGroundHeightAt(pos.x_, pos.z_);
		unit->updateHeight(y, maxTimeFrame);
	}
}

void Simulation::levelUp(QueueElement* done) const {
	levelsCommandList->add(new UpgradeCommand(
		Game::getPlayersMan()->getActivePlayer()->getId(),
		done->getId(),
		done->getType()
	));
}

void Simulation::updateBuildingQueues(const float time) const {
	for (auto build : *buildings) {
		auto done = build->updateQueue(time);
		if (done) {
			switch (done->getType()) {
			case ActionType::UNIT_CREATE:
			{
				auto center = enviroment->getCenter(build->getDeploy().value());

				creationCommandList->add(new CreationCommand(ObjectType::UNIT, done->getAmount(),
				                                             done->getId(), center,
				                                             build->getPlayer(),
				                                             Game::getPlayersMan()->
				                                             getPlayer(build->getPlayer())->
				                                             getLevelForUnit(done->getId())
				));
			}
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

void Simulation::updateQueues() const {
	updateBuildingQueues(maxTimeFrame);
	auto done = Game::getQueueManager()->update(maxTimeFrame);
	if (done) {
		switch (done->getType()) {
		case ActionType::BUILDING_LEVEL:
			levelUp(done);
			break;
		}
		delete done;
	}
}

void Simulation::dispose() const {
	simObjectManager->dispose();
}

void Simulation::save(SceneSaver& saver) const {
	saver.saveUnits(units);
	saver.saveBuildings(buildings);
	saver.saveResourceEntities(resources);
}

void Simulation::changeCoef(int i, int wheel) {
	force.changeCoef(i, wheel);
}

void Simulation::changeColorMode(ColorMode _colorMode) {
	colorScheme = _colorMode;
}

void Simulation::performStateAction(float timeStep) const {
	for (auto unit : *units) {
		StateManager::execute(unit, timeStep);
	}
	for (auto building : *buildings) {
		StateManager::executeChange(building);
	}
	for (auto resource : *resources) {
		StateManager::executeChange(resource);
	}
}

void Simulation::handleTimeInFrame(float timeStep) {
	countFrame();
	moveUnits(maxTimeFrame - (accumulateTime - timeStep));
	accumulateTime -= maxTimeFrame;
}

void Simulation::executeLists() const {
	levelsCommandList->execute();
	creationCommandList->execute();
}

void Simulation::initScene(SceneLoader& loader) const {
	loadEntities(loader);
	addTestEntities();
	executeLists();
}

void Simulation::initScene(NewGameForm* form) const {
	loadEntities(form);
	//addTestEntities();
	executeLists();
}

void Simulation::aiPlayers() const {
	aiManager->ai();
}

void Simulation::moveUnits(const float timeStep) const {
	for (auto unit : *units) {
		unit->move(timeStep);
	}
}

void Simulation::moveUnitsAndCheck(const float timeStep) const {
	for (auto unit : *units) {
		unit->move(timeStep);
		unit->checkAim();
		unit->changeColor(colorScheme);
	}
}

void Simulation::calculateForces() {
	DebugLineRepo::clear(DebugLineType::UNIT_LINES);
	DebugLineRepo::beginGeometry(DebugLineType::UNIT_LINES);

	for (auto unit : *units) {
		Urho3D::Vector2 newForce;
		switch (unit->getState()) {
		case UnitState::COLLECT:
			force.inCell(newForce, unit);
			break;
		case UnitState::ATTACK:
		{
			const auto neighbours = enviroment->getNeighbours(unit, unit->getMaxSeparationDistance());

			force.separationUnits(newForce, unit, neighbours);
			force.inCell(newForce, unit);
		}
		break;
		default:
		{
			const auto neighbours = enviroment->getNeighbours(unit, unit->getMaxSeparationDistance());

			force.separationUnits(newForce, unit, neighbours);
			force.separationObstacle(newForce, unit); //TODO mo¿ê to 
			force.destination(newForce, unit);
			force.formation(newForce, unit);
			force.escapeFromInvalidPosition(newForce, unit);
		}
		}

		auto& stats = force.stats();
		stats.result();

		unit->setAcceleration(newForce);
		unit->debug(DebugUnitType::AIM, stats);
	}
	DebugLineRepo::commit(DebugLineType::UNIT_LINES);
}
