#include "Simulation.h"
#include "Game.h"
#include "SimulationObjectManager.h"
#include "camera/CameraManager.h"
#include "commands/upgrade/UpgradeCommand.h"
#include "debug/DebugLineRepo.h"
#include "debug/DebugUnitType.h"
#include "env/Environment.h"
#include "hud/window/main_menu/new_game/NewGameForm.h"
#include "objects/PhysicalUtils.h"
#include "objects/building/Building.h"
#include "objects/queue/QueueActionType.h"
#include "objects/queue/QueueElement.h"
#include "objects/resource/ResourceEntity.h"
#include "objects/unit/SimColorMode.h"
#include "objects/unit/order/enums/UnitAction.h"
#include "player/Player.h"
#include "player/PlayersManager.h"
#include "player/ai/ActionCenter.h"
#include "scene/load/dbload_container.h"
#include "scene/load/SceneLoader.h"
#include "scene/save/SceneSaver.h"
#include "simulation/formation/FormationManager.h"
#include "stats/Stats.h"
#include "PerFrameAction.h"
#include "SimInfo.h"

Simulation::Simulation(Environment* enviroment): enviroment(enviroment) {
	simObjectManager = new SimulationObjectManager();
	Game::setActionCenter(new ActionCenter(simObjectManager));
	colorScheme = SimColorMode::BASIC;

	units = simObjectManager->getUnits();
	buildings = simObjectManager->getBuildings();
	resources = simObjectManager->getResources();
	simInfo = new SimInfo();
	DebugLineRepo::init(DebugLineType::UNIT_LINES);
}

Simulation::~Simulation() {
	delete simObjectManager;
	delete simInfo;
	delete Game::getActionCenter();
	Game::setActionCenter(nullptr);
}

void Simulation::clearNodesWithoutDelete() {
	simObjectManager->clearNodesWithoutDelete();
}

void Simulation::forceUpdateInfluenceMaps() const {
	enviroment->updateInfluenceUnits1(units);
	enviroment->updateInfluenceUnits2(units);
	enviroment->updateInfluenceResources(resources);
	enviroment->updateInfluenceOther(buildings);
	enviroment->updateQuadOther();
}

void Simulation::updateInfluenceMaps() const {
	if (PER_FRAME_ACTION.get(PerFrameAction::INFLUENCE_UNITS_1, currentFrame, secondsElapsed)) {
		enviroment->updateInfluenceUnits1(units);
	}
	if (PER_FRAME_ACTION.get(PerFrameAction::INFLUENCE_UNITS_2, currentFrame, secondsElapsed)) {
		enviroment->updateInfluenceUnits2(units);
	}
	if (PER_FRAME_ACTION.get(PerFrameAction::INFLUENCE_RESOURCES, currentFrame, secondsElapsed)) {
		enviroment->updateInfluenceResources(resources);
	}
	if (PER_FRAME_ACTION.get(PerFrameAction::INFLUENCE_OTHER, currentFrame, secondsElapsed)) {
		enviroment->updateInfluenceOther(buildings);
	}
	if (PER_FRAME_ACTION.get(PerFrameAction::INFLUENCE_QUAD_OTHER, currentFrame, secondsElapsed)) {
		enviroment->updateQuadOther();
	}
}

SimInfo* Simulation::update(float timeStep) {
	accumulateTime += updateTime(timeStep);
	simInfo->setIsRealFrame(false);
	while (accumulateTime >= TIME_PER_UPDATE) {
		//simObjectManager->dispose();
		//TODO bug a co jesli kilka razy sie wykona, moga byæ b³êdy jezeli cos umrze to poza petl¹ 
		simInfo->setFrame(currentFrame, secondsElapsed);

		Game::getActionCenter()->executeLists();

		selfAI();
		aiPlayers();

		Game::getActionCenter()->executeActions();

		calculateForces();
		applyForce(); //zmiany stanu

		moveUnitsAndCheck(TIME_PER_UPDATE); //zmiany stanu
		performStateAction(TIME_PER_UPDATE); //tutaj moga umierac w tym zmiany stanu
		executeStateTransition();
		updateQueues();
		updateInfluenceMaps(); //TODO odtad dalej mozna zrobic forka ?

		simObjectManager->dispose();
		simObjectManager->findToDispose();
		simInfo->setObjectsInfo(simObjectManager->getInfo());

		Game::getPlayersMan()->update(simInfo->getObjectsInfo());
		Game::getFormationManager()->update();
		Game::getStats()->save(PER_FRAME_ACTION.get(PerFrameAction::STAT_SAVE, currentFrame, secondsElapsed));

		accumulateTime -= TIME_PER_UPDATE;

		countFrame();
	}

	return simInfo;
}

void Simulation::tryToAttack(Unit* unit, float dist, UnitAction order,
                             const std::function<bool(Physical*)>& condition) const {
	toAction(unit, enviroment->getNeighboursFromTeamNotEq(unit, dist, unit->getTeam()), order, condition);
}

void Simulation::tryToCollect(Unit* unit) const {
	toAction(unit, enviroment->getResources(unit, 12), UnitAction::COLLECT, belowClose);
}

void Simulation::toAction(Unit* unit, std::vector<Physical*>* list, UnitAction order,
                          const std::function<bool(Physical*)>& condition) const {
	auto [closest, minDistance, indexToInteract] = unit->closestPhysical(list, condition);
	unit->toAction(closest, order, minDistance);
}

void Simulation::selfAI() const {
	if (PER_FRAME_ACTION.get(PerFrameAction::SELF_AI, currentFrame)) {
		for (auto unit : *units) {
			if (isFree(unit)) {
				if (StateManager::canChangeState(unit, unit->getActionState())) {
					switch (unit->getActionState()) {
					case UnitState::ATTACK:
						tryToAttack(unit, 12, UnitAction::ATTACK, belowClose);
						break;
					case UnitState::COLLECT:
						tryToCollect(unit);
						break;
					case UnitState::SHOT:
						tryToAttack(unit, 12, UnitAction::ATTACK, belowRange);
						break;
					}
				}
			}
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
		//Game::getActionCenter()->addUnits(UNITS_NUMBER * 30, 0, Urho3D::Vector2(20, -250), 0);
		//Game::getActionCenter()->addUnits(UNITS_NUMBER * 10, 4, Urho3D::Vector2(10, 240), 1);
		//Game::getActionCenter()->addUnits(UNITS_NUMBER * 300, 0, Urho3D::Vector2(10, 250), 1);
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

float Simulation::updateTime(float timeStep) const {
	if (SIM_GLOBALS.BENCHMARK_MODE) {
		return TIME_PER_UPDATE;
	}
	return timeStep;
}

void Simulation::countFrame() {
	++currentFrame;
	if (currentFrame >= FRAMES_IN_PERIOD) {
		currentFrame = 0;
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
			case QueueActionType::UNIT_CREATE: {
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
}

void Simulation::executeStateTransition() const {
	StateManager::executeChange(units);
	StateManager::executeChange(buildings);
	StateManager::executeChange(resources);
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
	if (PER_FRAME_ACTION.get(PerFrameAction::AI_ACTION, currentFrame)) {
		for (auto player : Game::getPlayersMan()->getAllPlayers()) {
			if (SIM_GLOBALS.TRAIN_MODE || Game::getPlayersMan()->getActivePlayer() != player) {
				player->aiAction();
			}
		}
	}

	if (PER_FRAME_ACTION.get(PerFrameAction::AI_ORDER, currentFrame)) {
		for (auto player : Game::getPlayersMan()->getAllPlayers()) {
			if (SIM_GLOBALS.TRAIN_MODE || Game::getPlayersMan()->getActivePlayer() != player) {
				player->aiOrder();
			}
		}
	}
}

void Simulation::moveUnitsAndCheck(const float timeStep) {
	auto camInfo = Game::getCameraManager()->getCamInfo(UPDATE_DRAW_DISTANCE);

	for (auto unit : *units) {
		const bool hasMoved = unit->move(timeStep, camInfo);

		unit->checkAim();
		if (hasMoved) {
			enviroment->update(unit);
		} else { unit->indexHasChangedReset(); }
	}
	enviroment->invalidateCaches();

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
		case UnitState::ATTACK: {
			const auto neighbours = enviroment->getNeighboursWithCache(unit, unit->getMaxSeparationDistance());

			force.separationUnits(newForce, unit, neighbours);
			force.inCell(newForce, unit);
		}
		break;
		default: {
			const auto neighbours = enviroment->getNeighboursWithCache(unit, unit->getMaxSeparationDistance());

			force.separationUnits(newForce, unit, neighbours);
			force.separationObstacle(newForce, unit);
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
