#pragma once
#include <array>
#include <span>
#include <magic_enum.hpp>

class Brain;
class Player;
class AiHistory;
struct db_nation;

enum class MilitaryInputIdx : unsigned char {
	PLAYER_SCORE,
	ENEMY_SCORE,
	ATTACK_SUM,
	DEFENCE_SUM,
	DIST_OUR_ARMY_OUR_BUILDING,
	DIST_OUR_ARMY_ENEMY_BUILDING,
	DIST_ENEMY_ARMY_OUR_BUILDING,
	DIST_ENEMY_ARMY_ENEMY_BUILDING,
	ARMY_COUNT,
	ENEMY_ARMY_COUNT,
	FREE_ARMY_COUNT,
	FREE_ARMY_ATTACK_SUM,
	ARMY_ATTACKING_RATIO,
	ARMY_DEFENDING_RATIO,
	MILITARY_URGENCY,
	ATTACK_URGENCY,
	OWN_INFANTRY_RATIO,
	OWN_RANGE_RATIO,
	OWN_CAVALRY_RATIO,
	ENEMY_INFANTRY_RATIO,
	ENEMY_RANGE_RATIO,
	ENEMY_CAVALRY_RATIO,
	RECENT_ATTACK_FAILURES,
	RECENT_DEFEND_FAILURES,
	RECENT_ATTACK_ACTIVITY,
	RECENT_DEFEND_ACTIVITY,
};

enum class MilitaryOutputIdx : unsigned char {
	ATTACK_RATIO,
	DEFEND_RATIO,
	ATTACK_STANCE,
	DEFEND_STANCE,
	PREFER_INFANTRY,
	PREFER_RANGE,
	PREFER_CAVALRY,
};

struct MilitaryOutput {
	float attackRatio;
	float defendRatio;
	float attackStance;
	float defendStance;
	float preferInfantry;
	float preferRange;
	float preferCavalry;
};

class MilitaryBrain {
public:
	explicit MilitaryBrain(db_nation* nation);
	MilitaryBrain(const MilitaryBrain&) = delete;

	MilitaryOutput decide(Player* player, Player* enemy,
	                       float militaryUrgency, float attackUrgency,
	                       const AiHistory* history);

private:
	Brain* brain;
	std::array<float, magic_enum::enum_count<MilitaryInputIdx>()> inputData{};
};
