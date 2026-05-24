#pragma once
#include <array>
#include <magic_enum.hpp>

class Brain;
class Player;
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
	FARM_COUNT,
	SPAWNER_COUNT,
	FOOD_DECAY_RATE,
	TOTAL_RES_BUILDINGS,
	RES_WO_BONUS,
	LACKING_FOOD,
	LACKING_WOOD,
	LACKING_STONE,
	LACKING_GOLD,
	ECONOMY_URGENCY,
	WORKER_URGENCY,
	EXPAND_URGENCY,
};

enum class EconomyOutputIdx : unsigned char {
	WORKER_ALLOCATION,
	FOOD_PRIORITY,
	WOOD_PRIORITY,
	STONE_PRIORITY,
	GOLD_PRIORITY,
	EXPAND_PRIORITY,
	RESOURCE_BUILDING_URGENCY,
	REASSIGN_WORKERS,
	NEED_MILL,
	NEED_SAWMILL,
	NEED_MINE_S,
	NEED_MINE_G,
	NEED_FARM,
	NEED_GRANARY,
	NEED_BANK,
	NEED_GOLD_REFINERY,
	NEED_STONE_REFINERY,
	NEED_TREE_NURSERY,
};

struct EconomyOutput {
	float workerAllocation;
	float foodPriority;
	float woodPriority;
	float stonePriority;
	float goldPriority;
	float expandPriority;
	float resourceBuildingUrgency;
	float reassignWorkers;
	float needMill;
	float needSawmill;
	float needMineS;
	float needMineG;
	float needFarm;
	float needGranary;
	float needBank;
	float needGoldRefinery;
	float needStoneRefinery;
	float needTreeNursery;
};

class EconomyBrain {
public:
	static constexpr int MAX_WORKERS_PER_TICK = 3;

	explicit EconomyBrain(db_nation* nation);
	EconomyBrain(const EconomyBrain&) = delete;

	EconomyOutput decide(Player* player, Player* enemy,
	                      const std::array<float, 4>& lackingPerResource,
	                      float economyUrgency, float workerUrgency, float expandUrgency);

private:
	Brain* brain;
	std::array<float, magic_enum::enum_count<EconomyInputIdx>()> inputData{};
};
