#pragma once

#include "db_insert_utils.h"
#include "objects/unit/Unit.h"
#include "utils/OtherUtils.h"

const std::vector<std::string> unit_columns =
		{"id_db", "hp_coef", "uid", "player", "level", "position_x", "position_z", "state", "velocity_x", "velocity_z"};

template <>
void bindRow<Unit>(sqlite3_stmt* stmt, const ParamMap& p, int precision, const Unit* x) {
	auto& pos = x->getPosition();
	auto vel = x->velocity;
	bindI(stmt, p[":id_db"], x->getDbId());
	bindI(stmt, p[":hp_coef"], (int)(x->getHp() * precision));
	bindU(stmt, p[":uid"], x->getUid());
	bindB(stmt, p[":player"], x->getPlayer());
	bindB(stmt, p[":level"], x->getLevelNum());
	bindI(stmt, p[":position_x"], x->position.x_ * precision);
	bindI(stmt, p[":position_z"], x->position.z_ * precision);
	bindB(stmt, p[":state"], castC(x->getState()));
	bindI(stmt, p[":velocity_x"], x->velocity.x_ * precision);
	bindI(stmt, p[":velocity_z"], x->velocity.x_ * precision);
}

const std::vector<std::string> building_columns =
		{"id_db", "hp_coef", "uid", "player", "level", "bucket_x", "bucket_y", "state", "next_state", "deploy_Idx"};

template <>
void bindRow<Building>(sqlite3_stmt* stmt, const ParamMap& p, int precision, const Building* x) {
	bindI(stmt, p[":id_db"], x->getIdDb());
	bindI(stmt, p[":hp_coef"], x->getHpCoef());
	bindI(stmt, p[":uid"], x->getUid());
	bindI(stmt, p[":player"], x->getPlayer());
	bindI(stmt, p[":level"], x->getLevel());
	bindI(stmt, p[":bucket_x"], x->getBucketX());
	bindI(stmt, p[":bucket_y"], x->getBucketY());
	bindI(stmt, p[":state"], x->getState());
	bindI(stmt, p[":next_state"], x->getNextState())
	bindI(stmt, p[":deploy_Idx"], x->getDeployIdx())
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
{"id", "is_active", "team", "nation", "name", "color",
 "buildingUid", "unitUid", "food", "wood", "stone", "gold"};

template <>
void bindRow<Player>(sqlite3_stmt* stmt, const ParamMap& p, int precision, const Player* x) {
	bindI(stmt, p[":id"], p->getId());
	bindI(stmt, p[":is_active"], p->isActive());
	bindI(stmt, p[":team"], p->getTeam());
	bindI(stmt, p[":nation"], p->getNation());
	bindI(stmt, p[":name"], p->getName()); // assume overload handles string
	bindI(stmt, p[":color"], p->getColor());
	bindI(stmt, p[":buildingUid"], p->getBuildingUid());
	bindI(stmt, p[":unitUid"], p->getUnitUid());
	bindI(stmt, p[":food"], p->getFood());
	bindI(stmt, p[":wood"], p->getWood());
	bindI(stmt, p[":stone"], p->getStone());
	bindI(stmt, p[":gold"], p->getGold());
}

const std::vector<std::string> config_columns = {"precision", "map", "size"};
