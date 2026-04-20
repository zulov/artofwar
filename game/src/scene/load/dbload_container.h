#pragma once
#include <vector>
#include <Urho3D/Container/Str.h>

#include "database/db_columns.h"
#include "database/db_utils.h"
#include "utils/DeleteUtils.h"

struct dbload_config {
	dbload_config(sqlite3_stmt* stmt) : precision(asInt(stmt, ConfigCol::precision)), map(asInt(stmt, ConfigCol::map)), size(asInt(stmt, ConfigCol::size)) {}

	int precision;
	int map;
	int size;
};

struct dbload_physical {
	float hp;
	unsigned uid;
	short id_db;
	char player;
	char level;
	char state;
	char nextState;

	dbload_physical(short idDb, float hp, unsigned uid, short player, short level, char state, char nextState) :
		hp(hp), uid(uid), id_db(idDb), player(player), level(level), state(state), nextState(nextState) {}
};

struct dbload_static : dbload_physical {
	short buc_x;
	short buc_y;

	dbload_static(short idDb, float hp, unsigned uid, char player, int bucX, int bucY, char level, char state,
	              char nextState) :
		dbload_physical(idDb, hp, uid, player, level, state, nextState),
		buc_x(bucX),
		buc_y(bucY) {}
};

struct dbload_unit : dbload_physical {
	float pos_x;
	float pos_z;
	float vel_x;
	float vel_z;

	using C = UnitCol;
	dbload_unit(sqlite3_stmt* stmt, int p) :
		dbload_unit(asShort(stmt, C::id_db), asItoF(stmt, C::hp_coef, p), asUI(stmt, C::uid), asByte(stmt, C::player),
		            asByte(stmt, C::level), asItoF(stmt, C::position_x, p), asItoF(stmt, C::position_z, p),
		            asByte(stmt, C::state), asItoF(stmt, C::velocity_x, p), asItoF(stmt, C::velocity_z, p)) {}

	dbload_unit(short idDb, float hp, unsigned uid, char player, char level, float posX, float posZ, char state,
	            float velX, float velZ) :
		dbload_physical(idDb, hp, uid, player, level, state, -1),
		pos_x(posX),
		pos_z(posZ),
		vel_x(velX),
		vel_z(velZ) {}
};

struct dbload_building : dbload_static {
	int deploy_idx;
	unsigned thingToInteract;
	unsigned short currentFrameState;
	//TODO std::vector<dbload_queue*>;
	using C = BuildingCol;
	dbload_building(sqlite3_stmt* stmt, int p) :
		dbload_building(
				asShort(stmt, C::id_db), asItoF(stmt, C::hp_coef, p), asUI(stmt, C::uid), asByte(stmt, C::player),
				asByte(stmt, C::level), asInt(stmt, C::bucket_x), asInt(stmt, C::bucket_y),
				asByte(stmt, C::state), asByte(stmt, C::next_state), asInt(stmt, C::deploy_Idx)) {}

	dbload_building(short idDb, float hpCoef, unsigned uid, char player, char level, int bucX, int bucY, char state,
	                char nextState, int deploy_idx) :
		dbload_static(idDb, hpCoef, uid, player, bucX, bucY, level, state, nextState),
		deploy_idx(deploy_idx) {}
};

struct dbload_resource : dbload_static {
	using C = ResourceCol;
	dbload_resource(sqlite3_stmt* stmt, int p) :
		dbload_resource(asShort(stmt, C::id_db), asItoF(stmt, C::hp_coef, p),
		                asUI(stmt, C::uid), asInt(stmt, C::bucket_x), asInt(stmt, C::bucket_y),
		                asByte(stmt, C::state), asByte(stmt, C::next_state)) {}

	dbload_resource(short idDb, float hpCoef, unsigned uid, int bucX, int bucY, char state, char nextState) :
		dbload_static(idDb, hpCoef, uid, -1, bucX, bucY, -1, state, nextState) {}
};

struct dbload_player {
	bool is_active;
	unsigned char id;
	unsigned char team;
	unsigned char nation;
	unsigned char color;
	Urho3D::String name;
	unsigned buildingUid;
	unsigned unitUid;
	float food;
	float wood;
	float stone;
	float gold;

	using C = PlayerCol;
	dbload_player(sqlite3_stmt* stmt, int p) :
		dbload_player(asUByte(stmt, C::id), asBool(stmt, C::is_active), asUByte(stmt, C::team), asUByte(stmt, C::nation),
		              asText(stmt, C::name), asUByte(stmt, C::color), asUI(stmt, C::buildingUid), asUI(stmt, C::unitUid),
		              asItoF(stmt, C::food, p), asItoF(stmt, C::wood, p),
		              asItoF(stmt, C::stone, p), asItoF(stmt, C::gold, p)) {}

	dbload_player(unsigned char id, bool isActive, unsigned char team, unsigned char nation, const char* name,
	              unsigned char color, unsigned buildingUid, unsigned unitUid, float food, float wood, float stone,
	              float gold) :
		is_active(isActive),
		id(id),
		team(team),
		nation(nation),
		color(color),
		name(name),
		buildingUid(buildingUid),
		unitUid(unitUid),
		food(food),
		wood(wood),
		stone(stone),
		gold(gold) {}
};

struct dbload_container {
	dbload_container() = default;

	~dbload_container() {
		delete config;
		clear_and_delete_vector(players);

		clear_and_delete_vector(units);
		clear_and_delete_vector(buildings);
		clear_and_delete_vector(resources);
	}

	dbload_config* config{};

	std::vector<dbload_player*>* players{};

	std::vector<dbload_unit*>* units{};
	std::vector<dbload_building*>* buildings{};
	std::vector<dbload_resource*>* resources{};
};
