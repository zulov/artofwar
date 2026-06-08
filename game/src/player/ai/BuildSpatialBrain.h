#pragma once
#include <array>
#include <span>
#include <magic_enum.hpp>
#include "env/influence/InfluenceManager.h"

class Brain;
class Player;
struct db_nation;

enum class BuildSpatialInputIdx : unsigned char {
	BUILDING_URGENCY,
	EXPAND_URGENCY,
	DEFENCE_BUILDING_URGENCY,
	BUILDINGS_COUNT,
	WORKERS_COUNT,
	ARMY_COUNT,
	GAME_TIME,
	PLAYER_SCORE,
	ENEMY_SCORE,
	RES_FOOD,
	RES_WOOD,
	RES_STONE,
	RES_GOLD,
};

enum class BuildSpatialOutputIdx : unsigned char {
	W_BUILDINGS_INFLUENCE,
	W_UNITS_INFLUENCE,
	W_ATTACK_SPEED,
	W_GATHER_FOOD,
	W_GATHER_WOOD,
	W_GATHER_STONE,
	W_GATHER_GOLD,
	W_ENEMY_BUILDINGS_INFLUENCE,
	W_ENEMY_UNITS_INFLUENCE,
	W_ENEMY_ATTACK_SPEED,
};

struct BuildSpatialOutput {
	std::array<float, AI_MAP_COUNT> weights;
};

// The output layer width, the output enum, and the influence-map weight array must agree.
static_assert(magic_enum::enum_count<BuildSpatialOutputIdx>() == AI_MAP_COUNT,
              "BuildSpatialOutputIdx count must match AI_MAP_COUNT");

class BuildSpatialBrain {
public:
	explicit BuildSpatialBrain(db_nation* nation);
	BuildSpatialBrain(const BuildSpatialBrain&) = delete;

	BuildSpatialOutput decide(Player* player, Player* enemy,
	                          float buildingUrgency, float expandUrgency, float defenceBuildingUrgency);

private:
	Brain* brain;
	std::array<float, magic_enum::enum_count<BuildSpatialInputIdx>()> inputData{};
};
