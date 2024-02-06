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
#include "PerFrameAction.h"
#include "objects/projectile/ProjectileManager.h"
#include "objects/unit/order/OrderUtils.h"

Simulation::Simulation(Environment* enviroment): enviroment(enviroment) {
	simObjectManager = new SimulationObjectManager();
	Game::setActionCenter(new ActionCenter(simObjectManager));
	colorScheme = SimColorMode::BASIC;

	units = simObjectManager->getUnits();
	buildings = simObjectManager->getBuildings();
	resources = simObjectManager->getResources();
	frameInfo = new FrameInfo();
	DebugLineRepo::init(DebugLineType::UNIT_LINES);
}

Simulation::~Simulation() {
	delete simObjectManager;
	delete frameInfo;
	delete Game::getActionCenter();
	Game::setActionCenter(nullptr);
}

void Simulation::clearNodesWithoutDelete() const {
	simObjectManager->clearNodesWithoutDelete();
}

void Simulation::updateInfluenceMaps(bool force) const {
	if (force || canUpdate (PerFrameAction::INFLUENCE_UNITS_1) ) {
		enviroment->updateInfluenceUnits1(units);
	}
	if (force || canUpdate(PerFrameAction::INFLUENCE_UNITS_2)) {
		enviroment->updateInfluenceUnits2(units);
	}
	if (force || canUpdate(PerFrameAction::INFLUENCE_RESOURCES)) {
		enviroment->updateInfluenceResources(resources);
	}
	if (force || canUpdate(PerFrameAction::INFLUENCE_OTHER)) {
		enviroment->updateInfluenceOther(buildings, units);
	}
	if (force || canUpdate(PerFrameAction::INFLUENCE_HISTORY_RESET)) {
		enviroment->updateInfluenceHistoryReset();
	}
	if (force || canUpdate(PerFrameAction::INFLUENCE_QUAD_OTHER)) {
		enviroment->updateQuadOther();
	}
	if (force || canUpdate(PerFrameAction::VISIBILITY)) {
		enviroment->updateVisibility(buildings, units, resources);
	}	
}

bool Simulation::canUpdate(PerFrameAction type) const {
	return PER_FRAME_ACTION.get(type, currentFrame, secondsElapsed);
}

FrameInfo* Simulation::update(float timeStep) {
	accumulateTime += updateTime(timeStep);
	frameInfo->setIsRealFrame(false);

	while (accumulateTime >= TIME_PER_UPDATE) {
		//TODO bug a co jesli kilka razy sie wykona, moga byc bledy np w control
		frameInfo->set(currentFrame, secondsElapsed);

		Game::getActionCenter()->executeLists();

		selfAI();
		aiPlayers();

		Game::getActionCenter()->executeActions();

		calculateForces();
		applyForce(); //zmiany stanu

		moveUnitsAndCheck(TIME_PER_UPDATE); //zmiany stanu
		ProjectileManager::update(TIME_PER_UPDATE); //TODO czy to dobre miejsce? mog¹ tu ginaæ
		performStateAction(TIME_PER_UPDATE); //tutaj moga umierac w tym zmiany stanu
		executeStateTransition();
		updateQueues();
		updateInfluenceMaps(false); 

		simObjectManager->dispose();
		simObjectManager->findToDispose();

		Game::getPlayersMan()->update();
		Game::getFormationManager()->update();

		accumulateTime -= TIME_PER_UPDATE;

		countFrame();
	}

	return frameInfo;
}

void Simulation::selfAI() const {
	const bool ifSelfAction = PER_FRAME_ACTION.get(PerFrameAction::SELF_AI, currentFrame);
	if (ifSelfAction) {
		for (const auto unit : *units) {
			if (isFree(unit)) {
				if (StateManager::canChangeState(unit, unit->getActionState())) {
					switch (unit->getActionState()) {
					case UnitState::ATTACK:
					case UnitState::SHOT:
						tryToAttack(unit, getCondition(unit->getDbUnit()));
						break;
					case UnitState::COLLECT:
						tryToCollect(unit);
						break;
					}
				}
			}
		}
	}
	for (const auto building : *buildings) {
		building->updateAi(ifSelfAction);
	}
}

std::function<bool(Physical*)> Simulation::getCondition(db_unit* dbUnit) const {
	if (dbUnit->typeRange) {
		if (dbUnit->typeMelee) {
			return belowCloseOrRange; //TODO bug? teraz to sie wyklucza?
		}
		return belowRange;
	}
	if (dbUnit->typeMelee) {
		return belowClose;
	}
	return alwaysFalse;
}

void Simulation::loadEntities(SceneLoader& loader) const {
	for (const auto unit : *loader.getData()->units) {
		simObjectManager->load(unit);
	}
	for (const auto resource : *loader.getData()->resource_entities) {
		simObjectManager->load(resource);
	}
	for (const auto building : *loader.getData()->buildings) {
		simObjectManager->load(building);
	}
	simObjectManager->refreshAllStatic();
}

void Simulation::addTestEntities() const {
	if constexpr (UNITS_NUMBER > 0) {
		auto b = Urho3D::Vector2(-185, -140);
		auto c = Urho3D::Vector2(-185, -150);
		//Game::getActionCenter()->addUnits(UNITS_NUMBER * 0.1f, 0, Urho3D::Vector2(0, 100), 0);
		//Game::getActionCenter()->addUnits(60, 1, Urho3D::Vector2(-5, 25), 0);
		//Game::getActionCenter()->addUnits(UNITS_NUMBER  * 0.1f, 2, Urho3D::Vector2(0, 70), 1);

		Game::getActionCenter()->addUnits(30, 1, b, 0);
		//Game::getActionCenter()->addUnits(10, 2, Urho3D::Vector2(0, 70), 1);

		Game::getActionCenter()->addBuilding(2, c, 1, true);
		//Game::getActionCenter()->addUnits(UNITS_NUMBER * 0.5f, 1, Urho3D::Vector2(300, 212), 0);
		//Game::getActionCenter()->addUnits(UNITS_NUMBER * 0.2f, 1, Urho3D::Vector2(290, 210), 0);
		//Game::getActionCenter()->addUnits(UNITS_NUMBER * 10, 4, Urho3D::Vector2(10, 240), 1);
		//Game::getActionCenter()->addUnits(UNITS_NUMBER * 300, 0, Urho3D::Vector2(10, 250), 1);
		//Game::getActionCenter()->addUnits(UNITS_NUMBER*10, 4, Urho3D::Vector2(-20, -200), 1);
		//Game::getActionCenter()->addUnits(UNITS_NUMBER * 5, 0, Urho3D::Vector2(-20, -20), 0);
		//Game::getActionCenter()->addResource(5, b);
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
		//TODO to przeniesc do mova? to moze byc problem gdy jest przesuwanie poza klatka
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

void Simulation::updateBuildingQueues() const {
	for (const auto build : *buildings) {
		const auto done = build->updateQueue();
		if (done) {
			switch (done->getType()) {
			case QueueActionType::UNIT_CREATE: {
				const auto center = enviroment->getCenter(build->getDeploy().value());
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
			case QueueActionType::BUILDING_CREATE:
				build->complete();
			}
			delete done;
		}
	}
}

void Simulation::updateQueues() const {
	updateBuildingQueues();
	for (auto player : Game::getPlayersMan()->getAllPlayers()) {
		const auto done = player->updateQueue();
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
	StateManager::reset();
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
	if (SIM_GLOBALS.NO_PLAYER_AI) { return; }
	if (PER_FRAME_ACTION.get(PerFrameAction::AI_ACTION, currentFrame)) {
		for (auto player : Game::getPlayersMan()->getAllPlayers()) {
			if (SIM_GLOBALS.ALL_PLAYER_AI || Game::getPlayersMan()->getActivePlayer() != player) {
				player->aiAction();
			}
		}
	}

	if (PER_FRAME_ACTION.get(PerFrameAction::AI_ORDER, currentFrame)) {
		for (auto player : Game::getPlayersMan()->getAllPlayers()) {
			if (SIM_GLOBALS.ALL_PLAYER_AI || Game::getPlayersMan()->getActivePlayer() != player) {
				player->aiOrder();
			}
		}
	}
}

void Simulation::moveUnitsAndCheck(const float timeStep) {
	const auto camInfo = Game::getCameraManager()->getCamInfo(UPDATE_DRAW_DISTANCE);

	for (const auto unit : *units) {
		const bool hasMoved = unit->move(timeStep, camInfo);

		unit->checkAim();
		if (hasMoved) {
			enviroment->update(unit);
		} else { unit->setIndexChanged(false); }
	}
	enviroment->invalidateCaches();

	if (colorSchemeChanged || colorScheme != SimColorMode::BASIC) {
		for (const auto unit : *units) {
			unit->changeColor(colorScheme);
		}
		colorSchemeChanged = false;
	}
}

void Simulation::calculateForces() {
	DebugLineRepo::clear(DebugLineType::UNIT_LINES);
	DebugLineRepo::beginGeometry(DebugLineType::UNIT_LINES);

	for (const auto unit : *units) {
		auto& stats = force.stats().vectorReset();

		Urho3D::Vector2 newForce;
		switch (unit->getState()) {
		case UnitState::COLLECT:
			force.inCell(newForce, unit);
			break;
		case UnitState::ATTACK: {
			//TODO improve getMaxSeparationDistance powino sie dodac jeszcze minimal dist
			const auto neighbours = enviroment->getNeighboursWithCache(unit, unit->getMaxSeparationDistance());

			force.separationUnits(newForce, unit, neighbours);
			force.inCell(newForce, unit);
		}
		break;
		default: {
			//TODO improve getMaxSeparationDistance powino sie dodac jeszcze minimal dist
			const auto neighbours = enviroment->getNeighboursWithCache(unit, unit->getMaxSeparationDistance());

			force.separationUnits(newForce, unit, neighbours);
			force.separationObstacle(newForce, unit);
			force.escapeFromInvalidPosition(newForce, unit);

			force.destOrFormation(newForce, unit);
		}
		}

		stats.result();

		unit->setAcceleration(newForce);
		if(!SIM_GLOBALS.HEADLESS) {
			unit->debug(DebugUnitType::AIM, stats); //TODO przeniesc do Controls
		}
	}
	DebugLineRepo::commit(DebugLineType::UNIT_LINES);
}
