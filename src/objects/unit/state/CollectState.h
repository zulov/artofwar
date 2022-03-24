#pragma once
#include "Game.h"
#include "State.h"
#include "player/Player.h"
#include "player/PlayersManager.h"
#include "player/Resources.h"
#include "env/Environment.h"
#include "objects/CellState.h"
#include "objects/resource/ResourceEntity.h"

class CollectState : public State {
public:
	CollectState(): State({
		UnitState::STOP, UnitState::DEFEND, UnitState::DEAD,
		UnitState::GO, UnitState::FOLLOW, UnitState::CHARGE
	}) { }

	~CollectState() = default;

	bool canStart(Unit* unit, const ActionParameter& parameter) override {
		if (parameter.isThingAlive()) {
			auto const indexesToUse = parameter.thingToInteract->getIndexesForUse(unit);
			return std::ranges::find(indexesToUse, unit->getMainGridIndex()) != indexesToUse.end();
		}
		return false;
	}

	void onStart(Unit* unit, const ActionParameter& parameter) override {
		auto const indexesToUse = parameter.thingToInteract->getIndexesForUse(unit);
		const auto found = std::ranges::find(indexesToUse, unit->getMainGridIndex());
		assert(found != indexesToUse.end());

		setStartData(unit, *found, parameter.thingToInteract);

		unit->lastActionThingId = parameter.thingToInteract->getId();
		unit->velocity = Urho3D::Vector2::ZERO;
		Game::getEnvironment()->updateCell(unit->getMainGridIndex(), 1, CellState::COLLECT);
	}

	void onEnd(Unit* unit) override {
		if (unit->isFirstThingAlive()) {
			unit->thingToInteract->reduceClose();
		}
		Game::getEnvironment()->updateCell(unit->indexToInteract, -1, CellState::NONE);

		unit->indexToInteract = -1;
	}

	void execute(Unit* unit, float timeStep) override {
		if (!unit->isFirstThingAlive() || !Game::getEnvironment()->cellInState(
			unit->getMainGridIndex(), CellState::COLLECT)) {
			StateManager::toDefaultState(unit);
			return;
		}
		const auto first = unit->thingToInteract;
		if (unit->indexChanged()) {
			first->reduceClose();
			unit->thingToInteract = nullptr;

			auto const indexesToUse = first->getIndexesForUse(unit);
			const auto found = std::ranges::find(indexesToUse, unit->getMainGridIndex());
			if (found != indexesToUse.end()) {
				setStartData(unit, *found, first);
			} else {
				StateManager::toDefaultState(unit);
				return;
			}
		}

		auto& resources = Game::getPlayersMan()->getPlayer(unit->player)->getResources();
		const auto resource = (ResourceEntity*)unit->thingToInteract;
		const auto bonus = Game::getEnvironment()->getBonuses(unit->player, resource);
		const auto [value, died] = resource->absorbAttack(unit->dbLevel->collect * bonus * timeStep);
		//resource->getIndexInInfluence()
		Game::getEnvironment()->addCollect(unit, resource->getId(), value);
		resources.add(resource->getId(), value);
	}
};
