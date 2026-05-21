#pragma once
#include <array>
#include <span>
#include <magic_enum.hpp>

class Brain;
class Player;
struct db_nation;

enum class MilitaryInputIdx : unsigned char {
	ATTACK_SUM,
	DEFENCE_ATTACK_SUM,
	DIST_OUR_ARMY_OUR_BUILDING,
	DIST_OUR_ARMY_ENEMY_BUILDING,
	DIST_ENEMY_ARMY_OUR_BUILDING,
	DIST_ENEMY_ARMY_ENEMY_BUILDING,
	ENEMY_SCORE,
	PLAYER_SCORE,
	UNITS_COUNT,
	ENEMY_UNITS_COUNT,
	ARMY_COUNT,
	FREE_ARMY_COUNT,
	MILITARY_URGENCY,
	ATTACK_URGENCY,
};

enum class MilitaryOutputIdx : unsigned char {
	ATTACK_WEIGHT,
	DEFEND_WEIGHT,
	ATTACK_TARGET,
	DEFEND_TARGET,
};

struct MilitaryOutput {
	float attackWeight;
	float defendWeight;
	float attackTarget;
	float defendTarget;
};

class MilitaryBrain {
public:
	explicit MilitaryBrain(db_nation* nation);
	MilitaryBrain(const MilitaryBrain&) = delete;

	MilitaryOutput decide(Player* player, Player* enemy,
	                       float militaryUrgency, float attackUrgency);

private:
	Brain* brain;
	std::array<float, magic_enum::enum_count<MilitaryInputIdx>()> inputData{};
};
