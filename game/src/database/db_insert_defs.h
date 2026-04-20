#pragma once

#include "Game.h"
#include "db_columns.h"
#include "db_insert_utils.h"
#include "objects/building/Building.h"
#include "objects/resource/ResourceEntity.h"
#include "objects/unit/Unit.h"
#include "player/Player.h"
#include "player/Resources.h"
#include "utils/OtherUtils.h"

const auto unit_columns = columnNames<UnitCol>();

template <>
void bindRow<Unit>(sqlite3_stmt* stmt, int precision, const Unit* x) {
	bindI(stmt, UnitCol::id_db, x->getDbId());
	bindI(stmt, UnitCol::hp_coef, (int)(x->getHp() * precision));
	bindU(stmt, UnitCol::uid, x->getUid());
	bindC(stmt, UnitCol::player, x->getPlayer());
	bindC(stmt, UnitCol::level, x->getLevelNum());
	bindI(stmt, UnitCol::position_x, x->position.x_ * precision);
	bindI(stmt, UnitCol::position_z, x->position.z_ * precision);
	bindC(stmt, UnitCol::state, castC(x->getState()));
	bindI(stmt, UnitCol::velocity_x, x->velocity.x_ * precision);
	bindI(stmt, UnitCol::velocity_z, x->velocity.y_ * precision);
}

const auto building_columns = columnNames<BuildingCol>();

template <>
void bindRow<Building>(sqlite3_stmt* stmt, int precision, const Building* x) {
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
	bindI(stmt, BuildingCol::deploy_Idx, x->deployIndex);
}

const auto resources_columns = columnNames<ResourceCol>();

template <>
void bindRow<ResourceEntity>(sqlite3_stmt* stmt, int precision, const ResourceEntity* x) {
	bindI(stmt, ResourceCol::id_db, x->getDbId());
	bindI(stmt, ResourceCol::hp_coef, (int)(x->getHp() * precision));
	bindU(stmt, ResourceCol::uid, x->getUid());
	const auto cordsCell = Game::getEnvironment()->getCords(x->indexInMainGrid);
	bindI(stmt, ResourceCol::bucket_x, cordsCell.x_);
	bindI(stmt, ResourceCol::bucket_y, cordsCell.y_);
	bindI(stmt, ResourceCol::state, castC(x->state));
	bindI(stmt, ResourceCol::next_state, castC(x->nextState));
}

const auto players_columns = columnNames<PlayerCol>();

template <>
void bindRow<Player>(sqlite3_stmt* stmt, int precision, const Player* x) {
	auto resVals = x->resources->getValues();
	bindUC(stmt, PlayerCol::id, x->getId());
	bindB(stmt, PlayerCol::is_active, x->active);
	bindUC(stmt, PlayerCol::team, x->team);
	bindUC(stmt, PlayerCol::nation, x->getNation());
	bindT(stmt, PlayerCol::name, x->name.CString());
	bindUC(stmt, PlayerCol::color, x->color);
	bindI(stmt, PlayerCol::buildingUid, x->currentBuildingUId);
	bindI(stmt, PlayerCol::unitUid, x->currentUnitUId);
	bindI(stmt, PlayerCol::food, resVals[0] * precision);
	bindI(stmt, PlayerCol::wood, resVals[1] * precision);
	bindI(stmt, PlayerCol::stone, resVals[2] * precision);
	bindI(stmt, PlayerCol::gold, resVals[3] * precision);
}

const auto config_columns = columnNames<ConfigCol>();
