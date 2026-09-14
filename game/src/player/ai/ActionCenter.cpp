#include "player/ai/ActionCenter.h"

#include <utility>

#include "Game.h"
#include "commands/action/BuildingActionCommand.h"
#include "commands/action/BuildingActionType.h"
#include "commands/action/GeneralActionCommand.h"
#include "commands/action/GeneralActionType.h"
#include "commands/action/ResourceActionCommand.h"
#include "commands/action/ResourceActionType.h"
#include "commands/action/UnitActionCommand.h"
#include "commands/creation/CreationCommand.h"
#include "commands/upgrade/UpgradeCommand.h"
#include "database/db_struct.h"
#include "env/Environment.h"
#include "objects/Physical.h"
#include "objects/building/Building.h"
#include "objects/resource/ResourceEntity.h"
#include "objects/unit/Unit.h"
#include "objects/unit/order/FormationOrder.h"
#include "objects/unit/order/GroupOrder.h"
#include "objects/unit/order/IndividualOrder.h"
#include "player/Player.h"
#include "player/PlayersManager.h"
#include "simulation/formation/FormationManager.h"

ActionCenter::ActionCenter(SimulationObjectManager* simulationObjectManager) :
	creation(simulationObjectManager), upgrade(simulationObjectManager) {}

void ActionCenter::add(UpgradeCommand* command) { upgrade.add(command); }

void ActionCenter::add(ResourceActionCommand* command) { action.add(command); }

void ActionCenter::add(BuildingActionCommand* command) { action.add(command); }

void ActionCenter::addUnitAction(UnitOrder* first) { action.add(new UnitActionCommand(first)); }

void ActionCenter::addUnitAction(UnitOrder* first, UnitOrder* second) {
	action.add(new UnitActionCommand(first), new UnitActionCommand(second));
}

void ActionCenter::add(GeneralActionCommand* command) { action.add(command); }

void ActionCenter::executeActions() { action.execute(); }

void ActionCenter::createAndUpgrade() {
	creation.execute();
	upgrade.execute();
}

bool ActionCenter::addUnits(unsigned number, short id, Urho3D::Vector2 position, char player) {
	auto level = Game::getPlayersMan()->getPlayer(player)->getUnitLevel(id)->level;
	return addUnits(number, id, position, player, level);
}

bool ActionCenter::addUnits(unsigned number, short id, Urho3D::Vector2& position, char player, int level) {
	auto command = creation.addUnits(number, id, position, player, level);
	if (command) {
		creation.add(command);
		return true;
	}
	return false;
}

bool ActionCenter::addBuilding(short id, Urho3D::Vector2& position, char player, bool force) {
	auto level = Game::getPlayersMan()->getPlayer(player)->getBuildingLevel(id)->level;
	return addBuilding(id, position, player, level, force);
}

bool ActionCenter::addBuilding(short id, Urho3D::Vector2& position, char player, int level, bool force) {
	CreationCommand* command{};
	if (force) {
		command = creation.addBuildingForce(id, position, player, level);
	} else {
		command = creation.addBuilding(id, position, player, level);
	}

	if (command) {
		creation.add(command);
		return true;
	}
	return false;
}

bool ActionCenter::addResource(short id, int index, float hp) {
	auto cords = Game::getEnvironment()->getCords(index);
	if (const auto command = creation.addResource(id, cords)) {
		command->setHp(hp);
		creation.add(command);
		return true;
	}

	return false;
}

std::vector<PendingCommandSaveData> ActionCenter::saveState() const {
	std::vector<PendingCommandSaveData> state;
	unsigned short index = 0;
	for (auto& item : creation.saveState(index)) { state.push_back(std::move(item)); }
	for (auto& item : upgrade.saveState(index)) { state.push_back(std::move(item)); }
	for (auto& item : action.saveState(index)) { state.push_back(std::move(item)); }
	return state;
}

void ActionCenter::loadState(const std::vector<PendingCommandSaveData>& state,
							 const std::unordered_map<unsigned, Physical*>& byUid) {
	for (const auto& item : state) {
		auto resolve = [&byUid](unsigned uid) -> Physical* {
			const auto it = byUid.find(uid);
			return it == byUid.end() ? nullptr : it->second;
		};
		switch (item.kind) {
		case PendingCommandKind::CREATION: {
			auto type = static_cast<ObjectType>(item.action);
			CreationCommand* command{};
			if (type == ObjectType::UNIT) {
				command = new CreationCommand(type, item.id, Urho3D::Vector2(item.x, item.z), item.level, item.player,
											  item.number);
			} else {
				command = new CreationCommand(type, item.id, Urho3D::UShortVector2(item.x, item.z), item.level,
											  item.player);
			}
			command->setHp(item.hp);
			creation.add(command);
			break;
		}
		case PendingCommandKind::UPGRADE:
			upgrade.add(new UpgradeCommand(item.player, item.id, static_cast<QueueActionType>(item.action)));
			break;
		case PendingCommandKind::INDIVIDUAL_ORDER:
			if (item.entityUids.empty()) {
				break;
			}
			if (auto* unit = resolve(item.entityUids[0]); unit && unit->getType() == ObjectType::UNIT) {
				if (item.targetUid) {
					if (auto* target = resolve(item.targetUid)) {
						addUnitAction(new IndividualOrder(static_cast<Unit*>(unit),
														  static_cast<UnitAction>(item.action), target, item.append));
					}
				} else {
					addUnitAction(new IndividualOrder(static_cast<Unit*>(unit), static_cast<UnitAction>(item.action),
													  Urho3D::Vector2(item.x, item.z), item.append));
				}
			}
			break;
		case PendingCommandKind::GROUP_ORDER: {
			std::vector<Unit*> units;
			for (const auto uid : item.entityUids) {
				if (auto* unit = resolve(uid); unit && unit->getType() == ObjectType::UNIT) {
					units.push_back(static_cast<Unit*>(unit));
				}
			}
			if (!units.empty()) {
				if (item.targetUid) {
					if (auto* target = resolve(item.targetUid)) {
						addUnitAction(new GroupOrder(units, static_cast<UnitActionType>(item.actionType), item.action,
													 target, item.append));
					}
				} else {
					addUnitAction(new GroupOrder(units, static_cast<UnitActionType>(item.actionType), item.action,
												 Urho3D::Vector2(item.x, item.z), item.append));
				}
			}
			break;
		}
		case PendingCommandKind::FORMATION_ORDER:
			if (auto* formation = Game::getFormationManager()->getFormation(item.formationId)) {
				if (item.targetUid) {
					if (auto* target = resolve(item.targetUid)) {
						addUnitAction(new FormationOrder(formation, item.action, target, item.append));
					}
				} else {
					auto position = Urho3D::Vector2(item.x, item.z);
					addUnitAction(new FormationOrder(formation, item.action, position, item.append));
				}
			}
			break;
		case PendingCommandKind::BUILDING_ACTION: {
			std::vector<Physical*> buildings;
			for (const auto uid : item.entityUids) {
				if (auto* entity = resolve(uid)) {
					buildings.push_back(entity);
				}
			}
			add(new BuildingActionCommand(buildings, static_cast<BuildingActionType>(item.action), item.id));
			break;
		}
		case PendingCommandKind::RESOURCE_ACTION: {
			std::vector<Physical*> resources;
			for (const auto uid : item.entityUids) {
				if (auto* entity = resolve(uid)) {
					resources.push_back(entity);
				}
			}
			add(new ResourceActionCommand(resources, static_cast<ResourceActionType>(item.action), item.player));
			break;
		}
		case PendingCommandKind::GENERAL_ACTION:
			add(new GeneralActionCommand(item.id, static_cast<GeneralActionType>(item.action), item.player));
			break;
		}
	}
}
