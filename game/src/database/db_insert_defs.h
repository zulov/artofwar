#pragma once

#include "Game.h"
#include "db_columns.h"
#include "db_insert_utils.h"
#include "env/Environment.h"
#include "objects/building/Building.h"
#include "objects/resource/ResourceEntity.h"
#include "objects/unit/ChargeData.h"
#include "objects/unit/Unit.h"
#include "objects/unit/aim/Aim.h"
#include "player/Player.h"
#include "player/Resources.h"
#include "scene/load/RuntimeSaveData.h"
#include "utils/OtherUtils.h"

template <>
inline void bindRow<Unit>(sqlite3_stmt* stmt, int precision, const Unit* x) {
	const auto runtime = x->captureRuntimeState();
	bindI(stmt, UnitCol::id_db, x->getDbId());
	bindI(stmt, UnitCol::hp_coef, (int)(x->getHp() * precision));
	bindU(stmt, UnitCol::uid, x->getUid());
	bindC(stmt, UnitCol::player, x->getPlayer());
	bindC(stmt, UnitCol::level, x->getLevelNum());
	bindI(stmt, UnitCol::position_x, x->position.x_ * precision);
	bindI(stmt, UnitCol::position_z, x->position.y_ * precision);
	bindC(stmt, UnitCol::state, castC(x->getState()));
	bindI(stmt, UnitCol::velocity_x, x->velocity.x_ * precision);
	bindI(stmt, UnitCol::velocity_z, x->velocity.y_ * precision);
	bindC(stmt, UnitCol::next_state, runtime.nextState);
	bindB(stmt, UnitCol::state_pending, runtime.stateChangePending);
	bindI(stmt, UnitCol::frame_state, runtime.currentFrameState);
	bindI(stmt, UnitCol::formation, runtime.formation);
	bindI(stmt, UnitCol::pos_in_state, runtime.posInState);
	bindScaledI(stmt, UnitCol::command_priority, runtime.commandPriority, precision);
	bindUC(stmt, UnitCol::command_center, runtime.commandCenter);
	bindScaledI(stmt, UnitCol::charge_energy, runtime.chargeEnergy, precision);
	bindU(stmt, UnitCol::target_uid, runtime.targetUid);
	bindU(stmt, UnitCol::pending_target_uid, runtime.pendingTargetUid);
	bindC(stmt, UnitCol::aim_kind, static_cast<char>(runtime.aim.kind));
	bindU(stmt, UnitCol::aim_target_uid, runtime.aim.targetUid);
	bindI(stmt, UnitCol::aim_current, runtime.aim.current);
	bindScaledI(stmt, UnitCol::aim_direction_x, runtime.aim.directionX, precision);
	bindScaledI(stmt, UnitCol::aim_direction_z, runtime.aim.directionZ, precision);
	bindC(stmt, UnitCol::pending_aim_kind, static_cast<char>(runtime.pendingAim.kind));
	bindU(stmt, UnitCol::pending_aim_target_uid, runtime.pendingAim.targetUid);
	bindI(stmt, UnitCol::pending_aim_current, runtime.pendingAim.current);
	bindScaledI(stmt, UnitCol::pending_aim_direction_x, runtime.pendingAim.directionX, precision);
	bindScaledI(stmt, UnitCol::pending_aim_direction_z, runtime.pendingAim.directionZ, precision);
}

template <>
inline void bindRow<Building>(sqlite3_stmt* stmt, int precision, const Building* x) {
	bindI(stmt, BuildingCol::id_db, x->getDbId());
	bindI(stmt, BuildingCol::hp_coef, (int)(x->getHp() * precision));
	bindU(stmt, BuildingCol::uid, x->getUid());
	bindC(stmt, BuildingCol::player, x->getPlayer());
	bindC(stmt, BuildingCol::level, x->getLevelNum());
	const auto cordsCell = Game::getEnvironment()->getCords(x->indexInMainGrid);
	bindI(stmt, BuildingCol::bucket_x, cordsCell.x_);
	bindI(stmt, BuildingCol::bucket_y, cordsCell.y_);
	bindI(stmt, BuildingCol::state, castC(x->state));
	bindI(stmt, BuildingCol::next_state, castC(x->nextState));
	bindU(stmt, BuildingCol::target_uid, x->thingToInteract ? x->thingToInteract->getUid() : 0);
	bindI(stmt, BuildingCol::frame_state, x->currentFrameState);
}

template <>
inline void bindRow<ResourceEntity>(sqlite3_stmt* stmt, int precision, const ResourceEntity* x) {
	bindI(stmt, ResourceCol::id_db, x->getDbId());
	bindI(stmt, ResourceCol::hp_coef, (int)(x->getHp() * precision));
	bindU(stmt, ResourceCol::uid, x->getUid());
	const auto cordsCell = Game::getEnvironment()->getCords(x->indexInMainGrid);
	bindI(stmt, ResourceCol::bucket_x, cordsCell.x_);
	bindI(stmt, ResourceCol::bucket_y, cordsCell.y_);
	bindI(stmt, ResourceCol::state, castC(x->state));
	bindI(stmt, ResourceCol::next_state, castC(x->nextState));
}

template <>
inline void bindRow<Player>(sqlite3_stmt* stmt, int precision, const Player* x) {
	auto resVals = x->resources->getValues();
	const auto resourceState = x->resources->saveState(x->id);
	bindUC(stmt, PlayerCol::id, x->getId());
	bindB(stmt, PlayerCol::is_active, x->active);
	bindUC(stmt, PlayerCol::team, x->team);
	bindUC(stmt, PlayerCol::nation, x->getNation());
	bindI(stmt, PlayerCol::buildingUid, x->currentBuildingUId);
	bindI(stmt, PlayerCol::unitUid, x->currentUnitUId);
	bindI(stmt, PlayerCol::food, resVals[cast(ResourceType::FOOD)] * precision);
	bindI(stmt, PlayerCol::wood, resVals[cast(ResourceType::WOOD)] * precision);
	bindI(stmt, PlayerCol::stone, resVals[cast(ResourceType::STONE)] * precision);
	bindI(stmt, PlayerCol::gold, resVals[cast(ResourceType::GOLD)] * precision);
	bindScaledI(stmt, PlayerCol::gather_food, resourceState.gatherSpeeds1s[0], precision);
	bindScaledI(stmt, PlayerCol::gather_wood, resourceState.gatherSpeeds1s[1], precision);
	bindScaledI(stmt, PlayerCol::gather_stone, resourceState.gatherSpeeds1s[2], precision);
	bindScaledI(stmt, PlayerCol::gather_gold, resourceState.gatherSpeeds1s[3], precision);
	bindScaledI(stmt, PlayerCol::pending_gather_food, resourceState.sumGatherSpeed[0], precision);
	bindScaledI(stmt, PlayerCol::pending_gather_wood, resourceState.sumGatherSpeed[1], precision);
	bindScaledI(stmt, PlayerCol::pending_gather_stone, resourceState.sumGatherSpeed[2], precision);
	bindScaledI(stmt, PlayerCol::pending_gather_gold, resourceState.sumGatherSpeed[3], precision);
	bindScaledI(stmt, PlayerCol::sum_food, resourceState.sumValues[0], precision);
	bindScaledI(stmt, PlayerCol::sum_wood, resourceState.sumValues[1], precision);
	bindScaledI(stmt, PlayerCol::sum_stone, resourceState.sumValues[2], precision);
	bindScaledI(stmt, PlayerCol::sum_gold, resourceState.sumValues[3], precision);
}

template <>
inline void bindRow<ConfigSaveData>(sqlite3_stmt* stmt, int, const ConfigSaveData* x) {
	bindI(stmt, ConfigCol::precision, x->precision);
	bindI(stmt, ConfigCol::map, x->map);
	bindI(stmt, ConfigCol::size, x->size);
	bindU(stmt, ConfigCol::total_ticks, x->totalTicks);
	bindB(stmt, ConfigCol::rdn_present, x->randomPresent);
	bindU(stmt, ConfigCol::rdn_seed, x->random.seed);
	bindI(stmt, ConfigCol::rdn_ai_idx, x->random.floatIndexes[0]);
	bindI(stmt, ConfigCol::rdn_resource_rotation_idx, x->random.floatIndexes[1]);
	bindI(stmt, ConfigCol::rdn_collision_force_idx, x->random.floatIndexes[2]);
	bindI(stmt, ConfigCol::rdn_other_idx, x->random.floatIndexes[3]);
	bindI(stmt, ConfigCol::rdn_save_idx, x->random.intIndexes[0]);
	bindI(stmt, ConfigCol::rdn_player_name_idx, x->random.intIndexes[1]);
	bindI(stmt, ConfigCol::rdn_resource_node_idx, x->random.intIndexes[2]);
}

template <>
inline void bindRow<UnitOrderRow>(sqlite3_stmt* stmt, int precision, const UnitOrderRow* x) {
	bindU(stmt, UnitOrderCol::unit_uid, x->data.unitUid);
	bindI(stmt, UnitOrderCol::order_idx, x->orderIndex);
	bindC(stmt, UnitOrderCol::action, x->data.action);
	bindB(stmt, UnitOrderCol::append, x->data.append);
	bindB(stmt, UnitOrderCol::has_target, x->data.hasTarget);
	bindU(stmt, UnitOrderCol::target_uid, x->data.targetUid);
	bindScaledI(stmt, UnitOrderCol::x, x->data.x, precision);
	bindScaledI(stmt, UnitOrderCol::z, x->data.z, precision);
}

template <>
inline void bindRow<AimPathSaveData>(sqlite3_stmt* stmt, int, const AimPathSaveData* x) {
	bindU(stmt, AimPathCol::unit_uid, x->unitUid);
	bindT(stmt, AimPathCol::path, x->path);
	bindT(stmt, AimPathCol::pending_path, x->pendingPath);
}

template <>
inline void bindRow<QueueRow>(sqlite3_stmt* stmt, int, const QueueRow* x) {
	bindU(stmt, QueueCol::owner_id, x->data.ownerId);
	bindUC(stmt, QueueCol::owner_type, x->data.ownerType);
	bindI(stmt, QueueCol::order_idx, x->orderIndex);
	bindC(stmt, QueueCol::type, x->data.type);
	bindI(stmt, QueueCol::id, x->data.id);
	bindI(stmt, QueueCol::level_id, x->data.levelId);
	bindI(stmt, QueueCol::amount, x->data.amount);
	bindI(stmt, QueueCol::elapsed_ticks, x->data.elapsedTicks);
}

template <>
inline void bindRow<PlayerLevelSaveData>(sqlite3_stmt* stmt, int, const PlayerLevelSaveData* x) {
	bindUC(stmt, PlayerLevelCol::player, x->player);
	bindUC(stmt, PlayerLevelCol::type, x->type);
	bindI(stmt, PlayerLevelCol::id, x->id);
	bindC(stmt, PlayerLevelCol::level, x->level);
}

template <>
inline void bindRow<AiSaveData>(sqlite3_stmt* stmt, int precision, const AiSaveData* x) {
	bindUC(stmt, AiStateCol::player, x->player);
	bindI(stmt, AiStateCol::prev_score, x->prevScore);
	bindI(stmt, AiStateCol::prev_enemy_score, x->prevEnemyScore);
	bindU(stmt, AiStateCol::prev_units, x->prevUnits);
	bindScaledI(stmt, AiStateCol::prev_res_sum, x->prevResSum, precision);
	bindScaledI(stmt, AiStateCol::prev_gather_sum, x->prevGatherSum, precision);
	bindScaledI(stmt, AiStateCol::food_priority, x->foodPriority, precision);
	bindScaledI(stmt, AiStateCol::wood_priority, x->woodPriority, precision);
	bindScaledI(stmt, AiStateCol::stone_priority, x->stonePriority, precision);
	bindScaledI(stmt, AiStateCol::gold_priority, x->goldPriority, precision);
	bindScaledI(stmt, AiStateCol::pressure_our_army_our_econ, x->militaryPressure[0], precision);
	bindScaledI(stmt, AiStateCol::pressure_our_army_our_building, x->militaryPressure[1], precision);
	bindScaledI(stmt, AiStateCol::pressure_our_army_enemy_army, x->militaryPressure[2], precision);
	bindScaledI(stmt, AiStateCol::pressure_our_army_enemy_econ, x->militaryPressure[3], precision);
	bindScaledI(stmt, AiStateCol::pressure_our_army_enemy_building, x->militaryPressure[4], precision);
	bindScaledI(stmt, AiStateCol::pressure_our_army_battle, x->militaryPressure[5], precision);
	bindScaledI(stmt, AiStateCol::pressure_our_econ_our_building, x->militaryPressure[6], precision);
	bindScaledI(stmt, AiStateCol::pressure_our_econ_enemy_army, x->militaryPressure[7], precision);
	bindScaledI(stmt, AiStateCol::pressure_our_econ_enemy_econ, x->militaryPressure[8], precision);
	bindScaledI(stmt, AiStateCol::pressure_our_econ_enemy_building, x->militaryPressure[9], precision);
	bindScaledI(stmt, AiStateCol::pressure_our_econ_battle, x->militaryPressure[10], precision);
	bindScaledI(stmt, AiStateCol::pressure_our_building_enemy_army, x->militaryPressure[11], precision);
	bindScaledI(stmt, AiStateCol::pressure_our_building_enemy_econ, x->militaryPressure[12], precision);
	bindScaledI(stmt, AiStateCol::pressure_our_building_enemy_building, x->militaryPressure[13], precision);
	bindScaledI(stmt, AiStateCol::pressure_our_building_battle, x->militaryPressure[14], precision);
	bindScaledI(stmt, AiStateCol::pressure_enemy_army_enemy_econ, x->militaryPressure[15], precision);
	bindScaledI(stmt, AiStateCol::pressure_enemy_army_enemy_building, x->militaryPressure[16], precision);
	bindScaledI(stmt, AiStateCol::pressure_enemy_army_battle, x->militaryPressure[17], precision);
	bindScaledI(stmt, AiStateCol::pressure_enemy_econ_enemy_building, x->militaryPressure[18], precision);
	bindScaledI(stmt, AiStateCol::pressure_enemy_econ_battle, x->militaryPressure[19], precision);
	bindScaledI(stmt, AiStateCol::pressure_enemy_building_battle, x->militaryPressure[20], precision);
	bindScaledI(stmt, AiStateCol::lacking_food, x->lackingPerResource[0], precision);
	bindScaledI(stmt, AiStateCol::lacking_wood, x->lackingPerResource[1], precision);
	bindScaledI(stmt, AiStateCol::lacking_stone, x->lackingPerResource[2], precision);
	bindScaledI(stmt, AiStateCol::lacking_gold, x->lackingPerResource[3], precision);
}

template <>
inline void bindRow<AiWantRow>(sqlite3_stmt* stmt, int precision, const AiWantRow* x) {
	bindUC(stmt, AiWantCol::player, x->data.player);
	bindI(stmt, AiWantCol::order_idx, x->orderIndex);
	bindScaledI(stmt, AiWantCol::priority, x->data.priority, precision);
	bindScaledI(stmt, AiWantCol::base_priority, x->data.basePriority, precision);
	bindUC(stmt, AiWantCol::type, x->data.type);
	bindUC(stmt, AiWantCol::count, x->data.count);
	bindI(stmt, AiWantCol::specific_id, x->data.specificId);
	bindI(stmt, AiWantCol::age, x->data.age);
	bindUC(stmt, AiWantCol::reserve_ticks, x->data.reserveTicks);
	bindB(stmt, AiWantCol::active, x->data.active);
}

template <>
inline void bindRow<PendingCommandSaveData>(sqlite3_stmt* stmt, int precision, const PendingCommandSaveData* x) {
	bindI(stmt, PendingCommandCol::order_idx, x->order);
	bindC(stmt, PendingCommandCol::kind, static_cast<char>(x->kind));
	bindC(stmt, PendingCommandCol::action, x->action);
	bindC(stmt, PendingCommandCol::action_type, x->actionType);
	bindI(stmt, PendingCommandCol::id_db, x->id);
	bindC(stmt, PendingCommandCol::player, x->player);
	bindC(stmt, PendingCommandCol::level, x->level);
	bindU(stmt, PendingCommandCol::number, x->number);
	bindScaledI(stmt, PendingCommandCol::x, x->x, precision);
	bindScaledI(stmt, PendingCommandCol::z, x->z, precision);
	bindScaledI(stmt, PendingCommandCol::hp, x->hp, precision);
	bindU(stmt, PendingCommandCol::target_uid, x->targetUid);
	bindI(stmt, PendingCommandCol::formation_id, x->formationId);
	bindB(stmt, PendingCommandCol::append, x->append);
}

template <>
inline void bindRow<AiHistoryRow>(sqlite3_stmt* stmt, int, const AiHistoryRow* x) {
	bindUC(stmt, AiHistoryCol::player, x->data.player);
	bindB(stmt, AiHistoryCol::action, x->data.action);
	bindI(stmt, AiHistoryCol::order_idx, x->orderIndex);
	bindU(stmt, AiHistoryCol::tick, x->data.tick);
	bindC(stmt, AiHistoryCol::type, x->data.type);
	bindC(stmt, AiHistoryCol::result, x->data.result);
	bindU(stmt, AiHistoryCol::chosen_id, x->data.chosenId);
}

template <>
inline void bindRow<ProjectileSaveData>(sqlite3_stmt* stmt, int precision, const ProjectileSaveData* x) {
	bindU(stmt, ProjectileCol::aim_uid, x->aimUid);
	bindScaledI(stmt, ProjectileCol::percent_to_go, x->percentToGo, precision);
	bindScaledI(stmt, ProjectileCol::speed, x->speed, precision);
	bindScaledI(stmt, ProjectileCol::attack_val, x->attackVal, precision);
	bindC(stmt, ProjectileCol::player, x->player);
}

template <>
inline void bindRow<FormationSaveData>(sqlite3_stmt* stmt, int precision, const FormationSaveData* x) {
	bindI(stmt, FormationCol::id, x->id);
	bindC(stmt, FormationCol::state, x->state);
	bindC(stmt, FormationCol::type, x->type);
	bindScaledI(stmt, FormationCol::direction_x, x->directionX, precision);
	bindScaledI(stmt, FormationCol::direction_z, x->directionZ, precision);
}

template <>
inline void bindRow<FormationOrderRow>(sqlite3_stmt* stmt, int precision, const FormationOrderRow* x) {
	bindI(stmt, FormationOrderCol::formation_id, x->data.formationId);
	bindI(stmt, FormationOrderCol::order_idx, x->orderIndex);
	bindB(stmt, FormationOrderCol::pending, x->pending);
	bindI(stmt, FormationOrderCol::action, x->data.action);
	bindB(stmt, FormationOrderCol::append, x->data.append);
	bindB(stmt, FormationOrderCol::has_target, x->data.hasTarget);
	bindU(stmt, FormationOrderCol::target_uid, x->data.targetUid);
	bindScaledI(stmt, FormationOrderCol::x, x->data.x, precision);
	bindScaledI(stmt, FormationOrderCol::z, x->data.z, precision);
}

template <>
inline void bindRow<PendingCommandEntityRow>(sqlite3_stmt* stmt, int, const PendingCommandEntityRow* x) {
	bindI(stmt, PendingCommandEntityCol::command_idx, x->commandIndex);
	bindI(stmt, PendingCommandEntityCol::order_idx, x->orderIndex);
	bindU(stmt, PendingCommandEntityCol::uid, x->uid);
}
