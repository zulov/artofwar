#pragma once
#include <array>
#include <magic_enum.hpp>

class Brain;
class Player;
class AiHistory;
struct db_nation;

enum class EconomyInputIdx : unsigned char {
	PLAYER_SCORE,
	ENEMY_SCORE,
	RES_FOOD,
	RES_WOOD,
	RES_STONE,
	RES_GOLD,
	GATHER_FOOD,
	GATHER_WOOD,
	GATHER_STONE,
	GATHER_GOLD,
	FREE_WORKERS,
	WORKERS_COUNT,
	FOOD_STORAGE_RATIO,
	GOLD_STORAGE_RATIO,
	STONE_REFINE_RATIO,
	GOLD_REFINE_RATIO,
	STONE_REFINE_GAP,
	GOLD_REFINE_GAP,
	BONUS_COVERAGE_FOOD,
	BONUS_COVERAGE_WOOD,
	BONUS_COVERAGE_STONE,
	BONUS_COVERAGE_GOLD,
	NEARBY_FOOD_SUPPLY,
	NEARBY_WOOD_SUPPLY,
	TOTAL_RES_BUILDINGS,
	RES_WO_BONUS,
	LACKING_FOOD,
	LACKING_WOOD,
	LACKING_STONE,
	LACKING_GOLD,
	ECONOMY_URGENCY,
	WORKER_URGENCY,
	EXPAND_URGENCY,
	RECENT_COLLECT_FAILURES,

	// Upgrade inputs
	TECH_URGENCY,
	AVG_WORKER_LEVEL,
	AVG_RES_BUILDING_LEVEL,
	GAME_TIME,
};

enum class EconomyOutputIdx : unsigned char {
	WORKER_ALLOCATION,
	FOOD_PRIORITY,
	WOOD_PRIORITY,
	STONE_PRIORITY,
	GOLD_PRIORITY,
	EXPAND_PRIORITY,
	REASSIGN_WORKERS,
	NEED_BONUS_FOOD,
	NEED_BONUS_WOOD,
	NEED_BONUS_STONE,
	NEED_BONUS_GOLD,
	NEED_FOOD_SOURCE,
	NEED_FOOD_STORAGE,
	NEED_GOLD_STORAGE,
	NEED_GOLD_REFINE,
	NEED_STONE_REFINE,
	NEED_WOOD_SOURCE,

	// Upgrade outputs
	WORKER_UPGRADE_URGENCY,
	RES_BUILDING_UPGRADE_URGENCY,
};

struct EconomyOutput {//TODO better names
	float workerAllocation;
	unsigned char workerCount;//TODO move last

	float foodPriority;
	float woodPriority;
	float stonePriority;
	float goldPriority;

	float expandPriority;
	float reassignWorkers;
	float needBonusFood;
	float needBonusWood;
	float needBonusStone;
	float needBonusGold;
	float needFoodSource;
	float needFoodStorage;
	float needGoldStorage;
	float needGoldRefine;
	float needStoneRefine;
	float needWoodSource;

	// Upgrade urgencies
	float workerUpgradeUrgency;
	float resBuildingUpgradeUrgency;
};

class EconomyBrain {
public:
	static constexpr unsigned char MAX_WORKERS_PER_TICK = 3;

	explicit EconomyBrain(db_nation* nation);
	EconomyBrain(const EconomyBrain&) = delete;

	EconomyOutput decide(Player* player, Player* enemy,
	                      const std::array<float, 4>& lackingPerResource,
	                      float economyUrgency, float workerUrgency, float expandUrgency,
	                      float techUrgency, float gameTime,
	                      const AiHistory* history);

private:
	Brain* brain;
	db_nation* nation;
	std::array<float, magic_enum::enum_count<EconomyInputIdx>()> inputData{};
};
