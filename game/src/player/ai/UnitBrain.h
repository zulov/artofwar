#pragma once
#include <array>
#include <span>
#include <magic_enum.hpp>

class Brain;
class Player;
struct db_nation;

enum class UnitInputIdx : unsigned char {
	OUR_INFANTRY_RATIO,
	OUR_CAVALRY_RATIO,
	OUR_RANGE_RATIO,
	ENEMY_INFANTRY_RATIO,
	ENEMY_CAVALRY_RATIO,
	ENEMY_RANGE_RATIO,
	ARMY_COUNT,
	ENEMY_ARMY_COUNT,
	ATTACK_SUM,
	DEFENCE_ATTACK_SUM,
	ENEMY_SCORE,
	WORKERS_COUNT,
	UNIT_URGENCY,
	ATTACK_URGENCY,
};

enum class UnitOutputIdx : unsigned char {
	UNIT_PROFILE,  // indices 0..22 (23-dim profile vector)
	COUNT = 23,    // derived from unitUrgency, not a NN output
};

struct UnitOutput {
	std::array<float, 23> unitProfile;  // ideal unit property vector
	unsigned char count;                 // how many to produce
};

class UnitBrain {
public:
	static constexpr int MAX_UNITS_PER_TICK = 5;

	explicit UnitBrain(db_nation* nation);
	UnitBrain(const UnitBrain&) = delete;

	UnitOutput decide(Player* player, Player* enemy,
	                   float unitUrgency, float attackUrgency);

private:
	Brain* brain;
	std::array<float, magic_enum::enum_count<UnitInputIdx>()> inputData{};
};
