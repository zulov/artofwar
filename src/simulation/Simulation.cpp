#include "Simulation.h"
#include "Game.h"
#include "SimulationInfo.h"
#include "SimulationObjectManager.h"
#include "camera/CameraManager.h"
#include "commands/creation/CreationCommand.h"
#include "commands/upgrade/UpgradeCommand.h"
#include "debug/DebugLineRepo.h"
#include "debug/DebugUnitType.h"
#include "env/Environment.h"
#include "hud/window/main_menu/new_game/NewGameForm.h"
#include "objects/Physical.h"
#include "objects/PhysicalUtils.h"
#include "objects/building/Building.h"
#include "objects/queue/QueueActionType.h"
#include "objects/queue/QueueElement.h"
#include "objects/resource/ResourceEntity.h"
#include "objects/unit/SimColorMode.h"
#include "objects/unit/Unit.h"
#include "objects/unit/order/enums/UnitAction.h"
#include "objects/unit/state/StateManager.h"
#include "objects/unit/state/UnitState.h"
#include "player/Player.h"
#include "player/PlayersManager.h"
#include "player/ai/ActionCenter.h"
#include "player/ai/AiManager.h"
#include "scene/load/dbload_container.h"
#include "scene/load/SceneLoader.h"
#include "scene/save/SceneSaver.h"
#include "simulation/formation/FormationManager.h"
#include "stats/Stats.h"

constexpr bool BENCHMARK_MODE = true;
constexpr float UPDATE_DRAW_DISTANCE = 120.f;

constexpr float TIME_PER_UPDATE = 0.033333333f;
constexpr unsigned char FRAMES_IN_PERIOD = 1 / TIME_PER_UPDATE;

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
	delete Game::getActionCenter();
	Game::setActionCenter(nullptr);
}

void Simulation::updateInfluenceMaps() {
	if (currentFrameNumber % 2 == 0) {
		enviroment->updateInfluence1(units, buildings, resources);
	} else {
		enviroment->updateInfluence2(units, buildings);
	}
	enviroment->updateInfluence3();
}

SimulationInfo* Simulation::update(float timeStep) {
	simObjectManager->dispose();

	accumulateTime += updateTime(timeStep);
	while (accumulateTime >= TIME_PER_UPDATE) {//TODO bug a co jesli kilka razy sie wykona, moga byæ b³êdy jezeli cos umrze
		countFrame();

		Game::getActionCenter()->executeLists();
		selfAI();

		aiPlayers();

		Game::getActionCenter()->executeActions();
		enviroment->update(units);
		updateInfluenceMaps();

		calculateForces();
		applyForce();

		moveUnitsAndCheck(TIME_PER_UPDATE);
		performStateAction(TIME_PER_UPDATE);
		updateQueues();

		simObjectManager->prepareToDispose();
		simObjectManager->updateInfo(simulationInfo);
		simulationInfo->setCurrentFrame(currentFrameNumber);
		enviroment->removeFromGrids(simObjectManager->getToDispose());
		Game::getPlayersMan()->update(simulationInfo);
		Game::getFormationManager()->update();
		Game::getStats()->save(secondsElapsed % 10 == 0 && currentFrameNumber % FRAMES_IN_PERIOD == 0);
		//Every 10 seconds

		accumulateTime -= TIME_PER_UPDATE;
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
	unit->toAction(closest, minDistance, order);
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
		Game::getActionCenter()->addUnits(UNITS_NUMBER * 20, 0, Urho3D::Vector2(20, -220), 0);
		//Game::getActionCenter()->addUnits(UNITS_NUMBER * 10, 4, Urho3D::Vector2(10, 240), 1);
		//Game::getActionCenter()->addUnits(UNITS_NUMBER*10, 4, Urho3D::Vector2(-20, -200), 1);
		//Game::getActionCenter()->addUnits(UNITS_NUMBER * 5, 0, Urho3D::Vector2(-20, -20), 0);
		//Game::getActionCenter()->addResource(1, Urho3D::Vector2(i, j), 0);
	}
}

void Simulation::loadEntities(NewGameForm* form) const {
	for (const auto& player : form->players) {
		auto fejkPost = Urho3D::Vector2(); //TODO trzeba inne
		simObjectManager->addUnits(10, 1, fejkPost, player.id, 0);
	}
}

float Simulation::updateTime(float timeStep) const{
	if (BENCHMARK_MODE) {
		return TIME_PER_UPDATE;
	}
	return timeStep;
}

void Simulation::countFrame() {
	++currentFrameNumber;
	if (currentFrameNumber >= FRAMES_IN_PERIOD) {
		currentFrameNumber = 0;
		++secondsElapsed;
	}
}

void Simulation::applyForce() const {
	for (auto unit : *units) {
		unit->applyForce(TIME_PER_UPDATE);
		auto pos = unit->getPosition();
		//TODO to przeniesc do mova? to moze byc [rpblem gdy jest przesuwanie poza klatk¹
		const float y = enviroment->getGroundHeightAt(pos);
		unit->updateHeight(y, TIME_PER_UPDATE);
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
	updateBuildingQueues(TIME_PER_UPDATE);
	for (auto player : Game::getPlayersMan()->getAllPlayers()) {
		auto done = player->updateQueue(TIME_PER_UPDATE);
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
	if (currentFrameNumber % FRAMES_IN_PERIOD == 0) {
		aiManager->ai();
	}
}

void Simulation::moveUnitsAndCheck(const float timeStep) {
	auto pos = Game::getCameraManager()->getTargetPos();

	for (auto unit : *units) {
		unit->move(timeStep, pos, UPDATE_DRAW_DISTANCE);
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
		unit->debug(DebugUnitType::ALL_FORCE, stats); //TODO przeniesc do Controls
	}
	DebugLineRepo::commit(DebugLineType::UNIT_LINES);
}
