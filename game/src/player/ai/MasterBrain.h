#pragma once
#include <array>
#include <magic_enum.hpp>

class Brain;
class Player;
class AiHistory;
struct db_nation;

enum class MasterInputIdx : unsigned char {
	PLAYER_SCORE,
	ENEMY_SCORE,
	BUILDINGS_COUNT,
	WORKERS_COUNT,
	ARMY_COUNT,
	ENEMY_ARMY_COUNT,
	RES_FOOD,
	RES_WOOD,
	RES_STONE,
	RES_GOLD,
	GATHER_FOOD,
	GATHER_WOOD,
	GATHER_STONE,
	GATHER_GOLD,
	ATTACK_SUM,
	DEFENCE_SUM,
	DIST_OUR_ARMY_OUR_BUILDING,
	DIST_OUR_ARMY_ENEMY_BUILDING,
	DIST_ENEMY_ARMY_OUR_BUILDING,
	DIST_ENEMY_ARMY_ENEMY_BUILDING,
	TOTAL_LACKING,
	DELTA_SCORE,
	DELTA_ENEMY_SCORE,
	DELTA_UNITS,
	DELTA_RES,
	GAME_TIME,
	KD_RATIO,
	IN_COMBAT_RATIO,
	TECH_LEVEL,
	DELTA_GATHER_SPEED,
	RECENT_BUILD_FAILURES,
	RECENT_UNIT_FAILURES,
	RECENT_ATTACK_ACTIVITY,
	RECENT_DEFEND_ACTIVITY,

};

enum class MasterOutputIdx : unsigned char {
	WORKER_URGENCY,
	ECONOMY_URGENCY,
	BUILDING_URGENCY,
	UNIT_URGENCY,
	MILITARY_URGENCY,
	EXPAND_URGENCY,
	TECH_URGENCY,
	DEFENCE_BUILDING_URGENCY,
	ATTACK_URGENCY,
};

struct MasterOutput {
	float workerUrgency;
	float economyUrgency;
	float buildingUrgency;
	float unitUrgency;
	float militaryUrgency;
	float expandUrgency;
	float techUrgency;
	float defenceBuildingUrgency;
	float attackUrgency;
};

class MasterBrain {
public:
	explicit MasterBrain(db_nation* nation);
	MasterBrain(const MasterBrain&) = delete;

	MasterOutput decide(Player* player, Player* enemy, float totalLacking, const AiHistory* history);

private:
	void updateHistory(Player* player, Player* enemy);

	Brain* brain;
	std::array<float, magic_enum::enum_count<MasterInputIdx>()> inputData{};

	// History for deltas, should be loaded from save
	int prevScore = 0;
	int prevEnemyScore = 0;
	unsigned prevUnits = 0;
	float prevResSum = 0.f;
	float prevGatherSum = 0.f;
};
