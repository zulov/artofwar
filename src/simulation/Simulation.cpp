#include "Simulation.h"
#include "Game.h"
#include "SimulationInfo.h"
#include "SimulationObjectManager.h"
#include "commands/creation/CreationCommand.h"
#include "commands/upgrade/UpgradeCommand.h"
#include "debug/DebugLineRepo.h"
#include "debug/DebugUnitType.h"
#include "env/Environment.h"
#include "hud/window/main_menu/new_game/NewGameForm.h"
#include "objects/PhysicalUtils.h"
#include "objects/building/Building.h"
#include "objects/queue/QueueElement.h"
#include "objects/unit/SimColorMode.h"
#include "objects/unit/Unit.h"
#include "objects/unit/order/enums/UnitAction.h"
#include "objects/unit/state/StateManager.h"
#include "objects/unit/state/UnitState.h"
#include "player/Player.h"
#include "player/PlayersManager.h"
#include "player/ai/ActionCenter.h"
#include "player/ai/AiManager.h"
#include "scene/load/SceneLoader.h"
#include "scene/save/SceneSaver.h"
#include "simulation/formation/FormationManager.h"
#include "stats/Stats.h"

constexpr bool BENCHMARK_MODE = true;

Simulation::Simulation(Environment* enviroment): enviroment(enviroment) {
	simObjectManager = new SimulationObjectManager();
	Game::setActionCenter(new ActionCenter(simObjectManager));
	colorScheme = SimColorMode::BASIC;


	simulationInfo = new SimulationInfo();

	units = simObjectManager->getUnits();
	buildings = simObjectManager->getBuildings();
	resources = simObjectManager->getResources();
	DebugLineRepo::init(DebugLineType::UNIT_LINES);
}

Simulation::~Simulation() {
	delete simulationInfo;
	delete simObjectManager;
	Game::disposeActionCenter();
}

void Simulation::updateInfluenceMaps() const {
	if (currentFrameNumber % 2 == 0) {
		enviroment->updateInfluence1(units, buildings, resources);
	} else {
		enviroment->updateInfluence2(units, buildings);
	}
	enviroment->updateInfluence3();
}

SimulationInfo* Simulation::update(float timeStep) {
	simulationInfo->reset();
	timeStep = updateTime(timeStep);
	if (accumulateTime >= maxTimeFrame) {
		handleTimeInFrame(timeStep);

		Game::getActionCenter()->executeLists();
		selfAI();

		aiPlayers();

		Game::getActionCenter()->executeActions();
		enviroment->update(units);
		updateInfluenceMaps();

		calculateForces();
		applyForce();

		moveUnitsAndCheck(accumulateTime);

		performStateAction(timeStep);
		updateQueues();

		simObjectManager->prepareToDispose();
		simObjectManager->updateInfo(simulationInfo);
		simulationInfo->setCurrentFrame(currentFrameNumber);
		enviroment->removeFromGrids(simObjectManager->getToDispose());
		Game::getPlayersMan()->update(simulationInfo);
		Game::getFormationManager()->update();
		Game::getStats()->save();
	} else {
		moveUnits(timeStep);
	}
	return simulationInfo;
}

void Simulation::tryToAttack(Unit* unit, float dist, UnitAction order,
                             const std::function<bool(Physical*)>& condition) {
	toAction(unit, enviroment->getNeighboursFromTeamNotEq(unit, dist, unit->getTeam()), order, condition);
}

void Simulation::tryToCollect(Unit* unit) {
	toAction(unit, enviroment->getResources(unit, 12), UnitAction::COLLECT, belowClose);
}

void Simulation::toAction(Unit* unit, std::vector<Physical*>* list, UnitAction order,
                          const std::function<bool(Physical*)>& condition) {
	auto [closest, minDistance, indexToInteract] = unit->closestPhysical(list, condition);
	unit->toAction(closest, minDistance, indexToInteract, order);
}

void Simulation::selfAI() {
	for (auto unit : *units) {
		switch (unit->getState()) {
		case UnitState::CHARGE:
			unit->toCharge(enviroment->getNeighboursFromTeamNotEq(unit, 12, unit->getTeam()));
			break;
		case UnitState::STOP:
		case UnitState::MOVE:
			if (currentFrameNumber % 3 == 0 && unit->getFormation() == -1
				&& StateManager::checkChangeState(unit, unit->getActionState())) {
				switch (unit->getActionState()) {
				case UnitState::ATTACK:
					tryToAttack(unit, 12, UnitAction::ATTACK, belowClose);
					break ;
				case UnitState::COLLECT:
					tryToCollect(unit);
					break;
				case UnitState::SHOT:
					tryToAttack(unit, 12, UnitAction::ATTACK, belowRange);
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
		//Game::getActionCenter()->addUnits(UNITS_NUMBER * 100, 0, Urho3D::Vector2(20, -220), 0);
		//Game::getActionCenter()->addUnits(UNITS_NUMBER, 4, Urho3D::Vector2(10, -20), 1);
		//Game::getActionCenter()->addUnits(UNITS_NUMBER, 0, Urho3D::Vector2(-20, -10), 1);
		//Game::getActionCenter()->addUnits(UNITS_NUMBER * 5, 0, Urho3D::Vector2(-20, -20), 0);
		//Game::getActionCenter()->addResource(1, Urho3D::Vector2(i, j), 0);
	}
}

void Simulation::loadEntities(NewGameForm* form) const {
	for (const auto& player : form->players) {
		simObjectManager->addUnits(10, 1, Urho3D::Vector2(), player.id, 0);
	}
}

float Simulation::updateTime(float timeStep) {
	if constexpr (BENCHMARK_MODE || timeStep > maxTimeFrame) {
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

void Simulation::levelUp(QueueElement* done, char player) const {
	Game::getActionCenter()->add(new UpgradeCommand(
		player,
		done->getId(),
		done->getType()
	));
}

void Simulation::updateBuildingQueues(const float time) const {
	for (auto build : *buildings) {
		auto done = build->updateQueue(time);
		if (done) {
			switch (done->getType()) {
			case QueueActionType::UNIT_CREATE:
			{
				auto center = enviroment->getCenter(build->getDeploy().value());
				Game::getActionCenter()->addUnits(done->getAmount(),
				                                  done->getId(), center,
				                                  build->getPlayer());
			}
			break;
			case QueueActionType::UNIT_LEVEL:
			case QueueActionType::BUILDING_LEVEL:
			case QueueActionType::UNIT_UPGRADE:
				levelUp(done, build->getPlayer());
				break;
			}
			delete done;
		}
	}
}

void Simulation::updateQueues() const {
	updateBuildingQueues(maxTimeFrame);
	for (auto player : Game::getPlayersMan()->getAllPlayers()) {
		auto done = player->updateQueue(maxTimeFrame);
		if (done) {
			switch (done->getType()) {
			case QueueActionType::BUILDING_LEVEL:
				levelUp(done, player->getId());
				break;
			}
			delete done;
		}
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

void Simulation::changeColorMode(SimColorMode _colorMode) {
	colorSchemeChanged = true;
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

void Simulation::initScene(SceneLoader& loader) const {
	loadEntities(loader);
	addTestEntities();
	Game::getActionCenter()->executeLists();
}

void Simulation::initScene(NewGameForm* form) const {
	loadEntities(form);
	//addTestEntities();
	Game::getActionCenter()->executeLists();
}

void Simulation::aiPlayers() const {
	if (currentFrameNumber % 20 == 0) {
		aiManager->ai();
	}
}

void Simulation::moveUnits(const float timeStep) const {
	for (auto unit : *units) {
		unit->move(timeStep);
	}
}

void Simulation::moveUnitsAndCheck(const float timeStep) {
	for (auto unit : *units) {
		unit->move(timeStep);
		unit->checkAim();
	}
	if (colorSchemeChanged || colorScheme != SimColorMode::BASIC) {
		for (auto unit : *units) {
			unit->changeColor(colorScheme);
		}
		colorSchemeChanged = false;
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
		unit->debug(DebugUnitType::AIM, stats); //TODO przeniesc do Controls
	}
	DebugLineRepo::commit(DebugLineType::UNIT_LINES);
}
