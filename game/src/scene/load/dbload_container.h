#pragma once
#include <vector>
#include <Urho3D/Container/Str.h>
#include "utils/DeleteUtils.h"

struct dbload_config {
	dbload_config(const dbload_config&) = delete;
	dbload_config() = default;
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

	dbload_physical(short idDb, float hp, unsigned uid, short player, short level, char state, char nextState)
		: hp(hp), uid(uid), id_db(idDb), player(player), level(level), state(state), nextState(nextState) {}
};

struct dbload_static : dbload_physical {
	short buc_x;
	short buc_y;

	dbload_static(short idDb, float hp, unsigned uid, char player, int bucX, int bucY, char level, char state,
	              char nextState)
		: dbload_physical(idDb, hp, uid, player, level,state, nextState),
		  buc_x(bucX),
		  buc_y(bucY) {}
};

struct dbload_unit : dbload_physical {
	float pos_x;
	float pos_z;
	float vel_x;
	float vel_z;

	dbload_unit(int idDb, float hp, unsigned uid, char player, char level, float posX, float posZ, char state,
	            float velX, float velZ)
		: dbload_physical(idDb, hp, uid, player, level, state, -1),
		  pos_x(posX),
		  pos_z(posZ),
		  vel_x(velX),
		  vel_z(velZ) {}
};

struct dbload_building : dbload_static {
	int deploy_idx;

	dbload_building(short idDb, float hpCoef, unsigned uid, char player, char level, int bucX, int bucY, char state,
	                char nextState,
	                int deploy_idx)
		: dbload_static(idDb, hpCoef, uid, player, bucX, bucY, level, state, nextState),
		  deploy_idx(deploy_idx) {}
};

struct dbload_resource_entities : dbload_static {
	dbload_resource_entities(short idDb, float hpCoef, unsigned uid, int bucX, int bucY, char state,
	                         char nextState) : dbload_static(idDb, hpCoef, uid, -1, bucX, bucY, -1, state,
	                                                         nextState) {}
};

struct dbload_player {
	bool is_active;
	char id;
	char team;
	char nation;
	char color;
	Urho3D::String name;
	unsigned buildingUid;
	unsigned unitUid;

	dbload_player(int id, bool isActive, int team, int nation, char* name, int color, unsigned buildingUid,
	              unsigned unitUid)
		: is_active(isActive),
		  id(id),
		  team(team),
		  nation(nation),
		  color(color),
		  name(name),
		  buildingUid(buildingUid),
		  unitUid(unitUid) {}
};

struct dbload_resource {
	short player;
	float food;
	float wood;
	float stone;
	float gold;

	dbload_resource(short player, float food, float wood, float stone, float gold)
		: player(player),
		  food(food),
		  wood(wood),
		  stone(stone),
		  gold(gold) {}
};

struct dbload_container {
	dbload_container() {
		precision = 1;
		config = new dbload_config();
	}

	~dbload_container() {
		delete config;
		clear_and_delete_vector(players);
		clear_and_delete_vector(resources);

		clear_and_delete_vector(units);
		clear_and_delete_vector(buildings);
		clear_and_delete_vector(resource_entities);
	}

	int precision;
	dbload_config* config;

	std::vector<dbload_player*>* players{};
	std::vector<dbload_resource*>* resources{};

	std::vector<dbload_unit*>* units{};
	std::vector<dbload_building*>* buildings{};
	std::vector<dbload_resource_entities*>* resource_entities{};
};
