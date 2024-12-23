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
	short id_db;
	char player;
	char level;

	dbload_physical(short idDb, float hp, short player, short level)
		: id_db(idDb),
		  hp(hp),
		  player(player),
		  level(level) { }
};

struct dbload_static : dbload_physical {
	short buc_x;
	short buc_y;
	char state;
	char nextState;

	dbload_static(int idDb, float hp, int player, int bucX, int bucY, int level, int state,
	              int nextState)
		: dbload_physical(idDb, hp, player, level),
		  buc_x(bucX),
		  buc_y(bucY),
		  state(state),
		  nextState(nextState) { }
};

struct dbload_unit : dbload_physical {
	float pos_x;
	float pos_z;
	int state;
	float vel_x;
	float vel_z;

	dbload_unit(int idDb, float hp, int player, int level, float posX, float posZ, int state,
	            float velX, float velZ)
		: dbload_physical(idDb, hp, player, level),
		  pos_x(posX),
		  pos_z(posZ),
		  state(state),
		  vel_x(velX),
		  vel_z(velZ) { }
};

struct dbload_building : dbload_static {
	int deploy_idx;

	dbload_building(int idDb, float hpCoef, int player, int level, int bucX, int bucY, int state, int nextState,
	                int deploy_idx)
		: dbload_static(idDb, hpCoef, player, bucX, bucY, level, state, nextState),
		  deploy_idx(deploy_idx) { }
};

struct dbload_resource_entities : dbload_static {
	dbload_resource_entities(int idDb, float hpCoef, int bucX, int bucY, int state,
	                         int nextState) : dbload_static(idDb, hpCoef, -1, bucX, bucY, -1, state,
	                                                        nextState) { }
};

struct dbload_player {
	bool is_active;
	char id;
	char team;
	char nation;
	char color;
	Urho3D::String name;

	dbload_player(int id, bool isActive, int team, int nation, char* name, int color)
		: is_active(isActive),
		  id(id),
		  team(team),
		  nation(nation),
		  color(color),
		  name(name) { }
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
		  gold(gold) {
	}
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
