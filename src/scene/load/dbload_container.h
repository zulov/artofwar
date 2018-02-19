#pragma once
#include <vector>
#include "utils.h"

struct dbload_config
{
	int precision;
	int map;
};

struct dbload_physical
{
	int id_db;
	bool alive;
	float hp_coef;
	int player;


	dbload_physical(int idDb, bool alive, float hpCoef, int player)
		: id_db(idDb),
		alive(alive),
		hp_coef(hpCoef),
		player(player) {
	}
};

struct dbload_static : dbload_physical
{
	int buc_x;
	int buc_y;


	dbload_static(int idDb, bool alive, float hpCoef, int player, int bucX, int bucY)
		: dbload_physical(idDb, alive, hpCoef, player),
		buc_x(bucX),
		buc_y(bucY) {
	}
};

struct dbload_unit : dbload_physical
{
	float pos_x;
	float pos_z;
	int state;
	float vel_x;
	float vel_z;
	int aim_i;
	int level;


	dbload_unit(int idDb, bool alive, float hpCoef, int player, float posX, float posZ, int state, float velX, float velZ,
	            int aimI, int level)
		: dbload_physical(idDb, alive, hpCoef, player),
		pos_x(posX),
		pos_z(posZ),
		state(state),
		vel_x(velX),
		vel_z(velZ),
		aim_i(aimI),
		level(level) {
	}
};

struct dbload_building : dbload_static
{
	float target_x;
	float target_z;


	dbload_building(int idDb, bool alive, float hpCoef, int player, int bucX, int bucY, float targetX, float targetZ)
		: dbload_static(idDb, alive, hpCoef, player, bucX, bucY),
		target_x(targetX),
		target_z(targetZ) {
	}
};

struct dbload_resource_entities : dbload_static
{
	float amount;

	dbload_resource_entities(int idDb, bool alive, float hpCoef, int player, int bucX, int bucY, float amount)
		: dbload_static(idDb, alive, hpCoef, player, bucX, bucY),
		amount(amount) {
	}
};

struct dbload_player
{
	int id;
	bool is_active;
	int team;
	int nation;
	Urho3D::String name;
	int color;

	dbload_player(int id, bool isActive, int team, int nation, char* name, int color)
		: id(id),
		is_active(isActive),
		team(team),
		nation(nation),
		name(name),
		color(color) {
	}
};

struct dbload_resource
{
	int player;
	int resource;
	float amount;

	dbload_resource(int player, int resource, float amount)
		: player(player),
		resource(resource),
		amount(amount) {
	}
};

struct dbload_container
{
	dbload_container() {
		precision = 1;
		config = new dbload_config();
		players = new std::vector<dbload_player*>();
		resources = new std::vector<dbload_resource*>();

		units = new std::vector<dbload_unit*>();
		buildings = new std::vector<dbload_building*>();
		resource_entities = new std::vector<dbload_resource_entities*>();
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

	std::vector<dbload_player*>* players;
	std::vector<dbload_resource*>* resources;

	std::vector<dbload_unit*>* units;
	std::vector<dbload_building*>* buildings;
	std::vector<dbload_resource_entities*>* resource_entities;
};
