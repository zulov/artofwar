#pragma once
#include <array>
#include <span>
#include <magic_enum.hpp>
#include "env/influence/InfluenceManager.h"
#include "MilitaryBrain.h"

class Brain;
class Player;
struct db_nation;

enum class AttackSpatialInputIdx : unsigned char {
	ATTACK_RATIO,
	DEFEND_RATIO,
	ATTACK_STANCE,
	DEFEND_STANCE,
	ARMY_COUNT,
	FREE_ARMY_COUNT,
	ENEMY_ARMY_COUNT,
	PLAYER_SCORE,
	ENEMY_SCORE,
	ATTACK_SUM,
	DEFENCE_SUM,
	IN_COMBAT_RATIO,
	MILITARY_URGENCY,
	ATTACK_URGENCY,
};

enum class AttackSpatialOutputIdx : unsigned char {
	W_BUILDINGS_INFLUENCE,
	W_UNITS_INFLUENCE,
	W_ATTACK_SPEED,
	W_ENEMY_BUILDINGS_INFLUENCE,
	W_ENEMY_UNITS_INFLUENCE,
	W_ENEMY_ATTACK_SPEED,
};

struct AttackSpatialOutput {
	std::array<float, AI_ARMY_MAP_COUNT> weights;
};

// The output layer width, the output enum, and the influence-map weight array must agree.
static_assert(magic_enum::enum_count<AttackSpatialOutputIdx>() == AI_ARMY_MAP_COUNT,
              "AttackSpatialOutputIdx count must match AI_ARMY_MAP_COUNT");

class AttackSpatialBrain {
public:
	explicit AttackSpatialBrain(db_nation* nation);
	AttackSpatialBrain(const AttackSpatialBrain&) = delete;

	AttackSpatialOutput decide(Player* player, Player* enemy,
	                           const MilitaryOutput& milOut,
	                           float militaryUrgency, float attackUrgency);

private:
	Brain* brain;
	std::array<float, magic_enum::enum_count<AttackSpatialInputIdx>()> inputData{};
};
