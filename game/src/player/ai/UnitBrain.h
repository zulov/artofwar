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
	PREFER_INFANTRY,
	PREFER_RANGE,
	PREFER_CAVALRY,

	// Upgrade inputs
	TECH_URGENCY,
	AVG_UNIT_LEVEL,
	AVG_BUILDING_LEVEL,
	GAME_TIME,
};

static constexpr int UNIT_PROFILE_SIZE = 23;

enum class UnitOutputIdx : unsigned char {
	UNIT_PROFILE,                                // indices 0..22 (23-dim profile vector)
	UNIT_UPGRADE_URGENCY = UNIT_PROFILE_SIZE,    // single float: should we upgrade a unit?
	BUILDING_UPGRADE_URGENCY,                    // single float: should we upgrade a building?
	COUNT,                                       // derived from unitUrgency, not a NN output
};

struct UnitOutput {
	std::array<float, UNIT_PROFILE_SIZE> unitProfile;  // ideal unit property vector (reused for upgrade matching)
	unsigned char count;                                // how many to produce
	float unitUpgradeUrgency;                           // how much we want to upgrade a unit
	float buildingUpgradeUrgency;                       // how much we want to upgrade a building
};

class UnitBrain {
public:
	static constexpr int MAX_UNITS_PER_TICK = 5;

	explicit UnitBrain(db_nation* nation);
	UnitBrain(const UnitBrain&) = delete;

	UnitOutput decide(Player* player, Player* enemy,
	                   float unitUrgency, float attackUrgency,
	                   float preferInfantry, float preferRange, float preferCavalry,
	                   float techUrgency, float gameTime);

private:
	Brain* brain;
	db_nation* nation;
	std::array<float, magic_enum::enum_count<UnitInputIdx>()> inputData{};
};
