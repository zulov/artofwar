#pragma once

#include "Game.h"
#include "db_insert_utils.h"
#include "objects/building/Building.h"
#include "objects/resource/ResourceEntity.h"
#include "objects/unit/Unit.h"
#include "player/Player.h"
#include "utils/OtherUtils.h"

template <>
void bindRow<Physical>(sqlite3_stmt* stmt, const ParamMap& p, int precision, const Physical* x) {
	bindI(stmt, p[":id_db"], x->getDbId());
	bindI(stmt, p[":hp_coef"], (int)(x->getHp() * precision));
	bindU(stmt, p[":uid"], x->getUid());
	bindB(stmt, p[":player"], x->getPlayer());
	bindB(stmt, p[":level"], x->getLevelNum());
}

const std::vector<std::string> unit_columns =
		{"id_db", "hp_coef", "uid", "player", "level", "position_x", "position_z", "state", "velocity_x", "velocity_z"};

template <>
void bindRow<Unit>(sqlite3_stmt* stmt, const ParamMap& p, int precision, const Unit* x) {
	bindRow(stmt, p, precision, static_cast<const Physical*>(x));

	bindI(stmt, p[":position_x"], x->position.x_ * precision);
	bindI(stmt, p[":position_z"], x->position.z_ * precision);
	bindB(stmt, p[":state"], castC(x->getState()));
	bindI(stmt, p[":velocity_x"], x->velocity.x_ * precision);
	bindI(stmt, p[":velocity_z"], x->velocity.y_ * precision);
}

const std::vector<std::string> building_columns =
		{"id_db", "hp_coef", "uid", "player", "level", "bucket_x", "bucket_y", "state", "next_state", "deploy_Idx"};

template <>
void bindRow<Building>(sqlite3_stmt* stmt, const ParamMap& p, int precision, const Building* x) {
	const auto cordsCell = Game::getEnvironment()->getCords(x->indexInMainGrid);
	bindI(stmt, p[":id_db"], x->getIdDb());
	bindI(stmt, p[":hp_coef"], x->getHpCoef());
	bindI(stmt, p[":uid"], x->getUid());
	bindI(stmt, p[":player"], x->getPlayer());
	bindI(stmt, p[":level"], x->getLevel());
	bindI(stmt, p[":bucket_x"], cordsCell.x_);
	bindI(stmt, p[":bucket_y"], cordsCell.y_);
	bindI(stmt, p[":state"], castC(x->state));
	bindI(stmt, p[":next_state"], castC(x->nextState))
	bindI(stmt, p[":deploy_Idx"], x->deployIndex)
}

const std::vector<std::string> resources_columns =
		{"id_db", "hp_coef", "uid", "bucket_x", "bucket_y", "state", "next_state"};

template <>
void bindRow<ResourceEntity>(sqlite3_stmt* stmt, const ParamMap& p, int precision, const ResourceEntity* x) {
	bindI(stmt, p[":id_db"], x->getIdDb());
	bindI(stmt, p[":hp_coef"], x->getHpCoef());
	bindI(stmt, p[":uid"], x->getUid());
	bindI(stmt, p[":bucket_x"], x->getBucketX());
	bindI(stmt, p[":bucket_y"], x->getBucketY());
	bindI(stmt, p[":state"], x->getState());
	bindI(stmt, p[":next_state"], x->getNextState());
}

const std::vector<std::string> players_columns =
{"id", "is_active", "team", "nation", "name", "color", "buildingUid", "unitUid", "food", "wood", "stone", "gold"};

template <>
void bindRow<Player>(sqlite3_stmt* stmt, const ParamMap& p, int precision, const Player* x) {
	bindI(stmt, p[":id"], x->getId());
	bindI(stmt, p[":is_active"], x->isActive());
	bindI(stmt, p[":team"], x->getTeam());
	bindI(stmt, p[":nation"], x->getNation());
	bindI(stmt, p[":name"], x->getName()); // assume overload handles string
	bindI(stmt, p[":color"], x->getColor());
	bindI(stmt, p[":buildingUid"], x->getBuildingUid());
	bindI(stmt, p[":unitUid"], x->getUnitUid());
	bindI(stmt, p[":food"], x->getFood());
	bindI(stmt, p[":wood"], x->getWood());
	bindI(stmt, p[":stone"], x->getStone());
	bindI(stmt, p[":gold"], x->getGold());
}

const std::vector<std::string> config_columns = {"precision", "map", "size"};
