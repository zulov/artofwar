#pragma once
#include "db_insert_utils.h"
#include "objects/unit/Unit.h"
#include "utils/OtherUtils.h"

constexpr auto unit_columns = std::array{"id_db", "hp_coef", "uid", "player", "level", "position_x", "position_z",
                                         "state", "velocity_x", "velocity_z"};

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
