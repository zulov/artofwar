#pragma once
#include <array>
#include <cstddef>
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
	TECH_LEVEL,
};

enum class MilitaryCenterIdx : unsigned char {
	OUR_ARMY,
	OUR_ECON,
	OUR_BUILDING,
	ENEMY_ARMY,
	ENEMY_ECON,
	ENEMY_BUILDING,
	LAST_BATTLE,
};

static constexpr size_t MILITARY_CENTER_COUNT = magic_enum::enum_count<MilitaryCenterIdx>();
static constexpr size_t MILITARY_CENTER_PAIR_COUNT = MILITARY_CENTER_COUNT * (MILITARY_CENTER_COUNT - 1) / 2;

// One signed output per unordered pair. Positive means pressure from the first
// enum name to the second; negative means pressure in the opposite direction.
enum class MilitaryOutputIdx : unsigned char {
	OUR_ARMY_OUR_ECON,
	OUR_ARMY_OUR_BUILDING,
	OUR_ARMY_ENEMY_ARMY,
	OUR_ARMY_ENEMY_ECON,
	OUR_ARMY_ENEMY_BUILDING,
	OUR_ARMY_LAST_BATTLE,
	OUR_ECON_OUR_BUILDING,
	OUR_ECON_ENEMY_ARMY,
	OUR_ECON_ENEMY_ECON,
	OUR_ECON_ENEMY_BUILDING,
	OUR_ECON_LAST_BATTLE,
	OUR_BUILDING_ENEMY_ARMY,
	OUR_BUILDING_ENEMY_ECON,
	OUR_BUILDING_ENEMY_BUILDING,
	OUR_BUILDING_LAST_BATTLE,
	ENEMY_ARMY_ENEMY_ECON,
	ENEMY_ARMY_ENEMY_BUILDING,
	ENEMY_ARMY_LAST_BATTLE,
	ENEMY_ECON_ENEMY_BUILDING,
	ENEMY_ECON_LAST_BATTLE,
	ENEMY_BUILDING_LAST_BATTLE,
};

static_assert(magic_enum::enum_count<MilitaryOutputIdx>() == MILITARY_CENTER_PAIR_COUNT,
              "MilitaryOutputIdx must contain every unordered pair of MilitaryCenterIdx values");

// The network output vector is laid out as the center-pair pressures (indices
// [0 .. MILITARY_CENTER_PAIR_COUNT-1]) followed by the army-composition
// preferences below. Keep MILITARY_OUTPUT_COUNT in sync with the brain file width.
static constexpr size_t MILITARY_PREFER_COUNT = 3;
static constexpr size_t MILITARY_OUTPUT_COUNT = MILITARY_CENTER_PAIR_COUNT + MILITARY_PREFER_COUNT;

enum class MilitaryPreferIdx : unsigned char {
	INFANTRY = MILITARY_CENTER_PAIR_COUNT,
	RANGE,
	CAVALRY,
};

constexpr size_t militaryCenterPairIndex(MilitaryCenterIdx first, MilitaryCenterIdx second) {
	const int a = static_cast<int>(first);
	const int b = static_cast<int>(second);
	const int low = a < b ? a : b;
	const int high = a < b ? b : a;
	size_t index = 0;
	for (int i = 0; i < low; ++i) { index += MILITARY_CENTER_COUNT - static_cast<size_t>(i) - 1; }
	return index + static_cast<size_t>(high - low - 1);
}

struct MilitaryOutput {
	std::array<float, MILITARY_CENTER_PAIR_COUNT> centerPairPressure{};
	float preferInfantry = 0.f;
	float preferRange = 0.f;
	float preferCavalry = 0.f;

	float pressure(MilitaryCenterIdx from, MilitaryCenterIdx to) const {
		if (from == to) { return 0.f; }
		const float pairValue = centerPairPressure[militaryCenterPairIndex(from, to)];
		return static_cast<int>(from) < static_cast<int>(to)
			       ? (pairValue > 0.f ? pairValue : 0.f)
			       : (pairValue < 0.f ? -pairValue : 0.f);
	}
};

class MilitaryBrain {
public:
	explicit MilitaryBrain(db_nation* nation);
	MilitaryBrain(const MilitaryBrain&) = delete;

	MilitaryOutput decide(Player* player, Player* enemy,
	                       float militaryUrgency, float attackUrgency,
	                       float techLevel,
	                       const AiHistory* history);

private:
	Brain* brain;
	std::array<float, magic_enum::enum_count<MilitaryInputIdx>()> inputData{};
};
