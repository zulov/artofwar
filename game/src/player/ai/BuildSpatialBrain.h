#pragma once
#include <array>
#include <magic_enum.hpp>
#include "env/influence/InfluenceManager.h"

class Brain;
class Player;
struct db_building;
struct db_building_level;
struct db_nation;

enum class BuildSpatialInputIdx : unsigned char {
	EXPAND_URGENCY,
	BUILDINGS_COUNT,
	WORKERS_COUNT,
	ARMY_COUNT,
	GAME_TIME,
	PLAYER_SCORE,
	ENEMY_SCORE,
	// One-hot of BuildPlacementClass (must stay in the same order as the enum).
	CLASS_OTHER,
	CLASS_DEFENCE,
	CLASS_TECH,
	CLASS_UNITS,
	CLASS_RES_BONUS,
	CLASS_RES_CONVERT,
	CLASS_RES_SPAWNER,
	CLASS_RES_STORAGE,
	CLASS_RES_REFINE,
	RES_TYPE_FOOD,
	RES_TYPE_WOOD,
	RES_TYPE_STONE,
	RES_TYPE_GOLD,
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
	W_RES_NOT_BONUS,
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
	                          float expandUrgency,
	                          const db_building* building, const db_building_level* level);

private:
	Brain* brain;
	std::array<float, magic_enum::enum_count<BuildSpatialInputIdx>()> inputData{};
};
