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

template <>
void bindRow<Physical>(sqlite3_stmt* stmt, const ParamMap& p, int precision, const Physical* x) {
	bindI(stmt, p[":id_db"], x->getDbId());
	bindI(stmt, p[":hp_coef"], (int)(x->getHp() * precision));
	bindU(stmt, p[":uid"], x->getUid());
	if (x->getType() != ObjectType::RESOURCE) {
		bindC(stmt, p[":player"], x->getPlayer());
		bindC(stmt, p[":level"], x->getLevelNum());
	}
}

template <>
void bindRow<Static>(sqlite3_stmt* stmt, const ParamMap& p, int precision, const Static* x) {
	const auto cordsCell = Game::getEnvironment()->getCords(x->indexInMainGrid);
	bindI(stmt, p[":bucket_x"], cordsCell.x_);
	bindI(stmt, p[":bucket_y"], cordsCell.y_);
	bindI(stmt, p[":state"], castC(x->state));
	bindI(stmt, p[":next_state"], castC(x->nextState));
}

const auto unit_columns = columnNames<UnitCol>();

template <>
void bindRow<Unit>(sqlite3_stmt* stmt, const ParamMap& p, int precision, const Unit* x) {
	bindRow(stmt, p, precision, static_cast<const Physical*>(x));

	bindI(stmt, p[":position_x"], x->position.x_ * precision);
	bindI(stmt, p[":position_z"], x->position.z_ * precision);
	bindC(stmt, p[":state"], castC(x->getState()));
	bindI(stmt, p[":velocity_x"], x->velocity.x_ * precision);
	bindI(stmt, p[":velocity_z"], x->velocity.y_ * precision);
}

const auto building_columns = columnNames<BuildingCol>();

template <>
void bindRow<Building>(sqlite3_stmt* stmt, const ParamMap& p, int precision, const Building* x) {
	bindRow(stmt, p, precision, static_cast<const Physical*>(x));
	bindRow(stmt, p, precision, static_cast<const Static*>(x));

	bindI(stmt, p[":deploy_Idx"], x->deployIndex);
}

const auto resources_columns = columnNames<ResourceCol>();

template <>
void bindRow<ResourceEntity>(sqlite3_stmt* stmt, const ParamMap& p, int precision, const ResourceEntity* x) {
	bindRow(stmt, p, precision, static_cast<const Physical*>(x));
	bindRow(stmt, p, precision, static_cast<const Static*>(x));
}

const auto players_columns = columnNames<PlayerCol>();

template <>
void bindRow<Player>(sqlite3_stmt* stmt, const ParamMap& p, int precision, const Player* x) {
	auto resVals = x->resources->getValues();
	bindUC(stmt, p[":id"], x->getId());
	bindB(stmt, p[":is_active"], x->active);
	bindUC(stmt, p[":team"], x->team);
	bindUC(stmt, p[":nation"], x->getNation());
	bindT(stmt, p[":name"], x->name.CString()); // assume overload handles string
	bindUC(stmt, p[":color"], x->color);
	bindI(stmt, p[":buildingUid"], x->currentBuildingUId);
	bindI(stmt, p[":unitUid"], x->currentUnitUId);
	bindI(stmt, p[":food"], resVals[0] * precision);
	bindI(stmt, p[":wood"], resVals[1] * precision);
	bindI(stmt, p[":stone"], resVals[2] * precision);
	bindI(stmt, p[":gold"], resVals[3] * precision);
}

const auto config_columns = columnNames<ConfigCol>();
